/*
 ____ _____ _____  ____
|    |  |     |   |    |   k3serializer for Modern C++
|    |  |     |   |    |   version 0.0.1
|____|  |	  |   |____|   https://github.com/OttoX/k3serializer

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2019-2020 otto.x

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include <cstdint>
#include <type_traits>
#include <string.h>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>
#include <unordered_map>

namespace port
{
	static constexpr const bool kLittleEndian = true;
}

class K3SerializerBase
{
protected:
	static int VarintLength(uint64_t v);
	static void EncodeFixed32(char* buf, uint32_t v);
	static char* EncodeVarint32(char* dst, uint32_t v);
	static const char* GetVarint32Ptr(const char* p, const char* limit, uint32_t* v);
	static const char* GetVarint32PtrFallback(const char* p, const char* limit, uint32_t* v);
	static char* EncodeVarint64(char* dst, uint64_t v);
	static void EncodeFixed64(char* buf, uint64_t v);
	static const char* GetVarint64Ptr(const char* p, const char* limit, uint64_t* v);
	static inline uint32_t DecodeFixed32(const char* ptr) {
		if constexpr (port::kLittleEndian) {
			// Load the raw bytes
			uint32_t result;
			memcpy(&result, ptr, sizeof(result));  // gcc optimizes this to a plain load
			return result;
		}
		else {
			return ((static_cast<uint32_t>(static_cast<unsigned char>(ptr[0])))
				| (static_cast<uint32_t>(static_cast<unsigned char>(ptr[1])) << 8)
				| (static_cast<uint32_t>(static_cast<unsigned char>(ptr[2])) << 16)
				| (static_cast<uint32_t>(static_cast<unsigned char>(ptr[3])) << 24));
		}
	}
	static inline uint64_t DecodeFixed64(const char* ptr) {
		if (port::kLittleEndian) {
			// Load the raw bytes
			uint64_t result;
			memcpy(&result, ptr, sizeof(result));  // gcc optimizes this to a plain load
			return result;
		}
		else {
			uint64_t lo = DecodeFixed32(ptr);
			uint64_t hi = DecodeFixed32(ptr + 4);
			return (hi << 32) | lo;
		}
	}
};

class K3SerializerByte : public K3SerializerBase
{
	static void PutChar(std::string& dst, char v) { dst += v; }
	static bool GetChar(std::string_view& input, char* v);
};

class K3SerializerVarint32 : public K3SerializerBase
{
public:
	static void PutVarint32(std::string& dst, uint32_t v);
	static bool GetVarint32(std::string_view& input, uint32_t* v);
};

class K3SerializerFixed32 : public K3SerializerBase
{
public:
	static void PutFixed32(std::string& dst, uint32_t v);
	static bool GetFixed32(std::string_view& input, uint32_t* v);
};

class K3SerializerVarint64 : public K3SerializerBase
{
public:
	static void PutVarint64(std::string& dst, uint64_t v);
	static bool GetVarint64(std::string_view& input, uint64_t* v);
};

class K3SerializerFixed64 : public K3SerializerBase
{
public:
	static void PutFixed64(std::string& dst, uint64_t v);
	static bool GetFixed64(std::string_view& input, uint64_t* v);
};

template<typename T, typename=std::enable_if_t<std::is_enum_v<T>>>
class K3SerializerEnum : public K3SerializerVarint32
{
public:
	static void PutValue(std::string& dst, T v)
	{
		PutVarint32(dst, static_cast<uint32_t>(v));
	}
	static bool GetValue(std::string_view& src, T& v)
	{
		return GetVarint32(src, reinterpret_cast<uint32_t*>(&v));
	}
};

//class K3Serializer;

template<typename T>
class K3Serializer;

template<>
class K3Serializer<int64_t> : public K3SerializerVarint64 {
public:
	static void PutValue(std::string& dst, int64_t v)
	{
		PutVarint64(dst, v);
	}
	static bool GetValue(std::string_view& src, int64_t& v)
	{
		return GetVarint64(src, reinterpret_cast<uint64_t*>(&v));
	}
};
template<>
class K3Serializer<uint64_t> : public K3SerializerVarint64 {
public:
	static void PutValue(std::string& dst, uint64_t v)
	{
		PutVarint64(dst, v);
	}
	static bool GetValue(std::string_view& src, uint64_t& v)
	{
		return GetVarint64(src, &v);
	}
};

template<>
class K3Serializer<int> : public K3SerializerVarint32 {
public:
	static void PutValue(std::string& dst, int v)
	{
		PutVarint32(dst, v);
	}
	static bool GetValue(std::string_view& src, int& v)
	{
		return GetVarint32(src, reinterpret_cast<uint32_t*>(&v));
	}
};
template<>
class K3Serializer<uint32_t> : public K3SerializerVarint32 {
public:
	static void PutValue(std::string& dst, uint32_t v)
	{
		PutVarint32(dst, v);
	}
	static bool GetValue(std::string_view& src, uint32_t& v)
	{
		return GetVarint32(src, &v);
	}
};
template<>
class K3Serializer<int16_t> : public K3SerializerVarint32 {
public:
	static void PutValue(std::string& dst, int16_t v)
	{
		PutVarint32(dst, v);
	}
	static bool GetValue(std::string_view& src, int16_t& v)
	{
		return GetVarint32(src, reinterpret_cast<uint32_t*>(&v));
	}
};
template<>
class K3Serializer<uint16_t> : public K3SerializerVarint32 {
public:
	static void PutValue(std::string& dst, uint16_t v)
	{
		PutVarint32(dst, v);
	}
	static bool GetValue(std::string_view& src, uint16_t& v)
	{
		return GetVarint32(src, reinterpret_cast<uint32_t*>(&v));
	}
};

template<>
class K3Serializer<float> : public K3SerializerFixed32 {
public:
	static void PutValue(std::string& dst, float v)
	{
		union { float f; uint32_t i; };
		f = v;
		PutFixed32(dst, i);
	}
	static bool GetValue(std::string_view& src, float& v)
	{
		union { float f; uint32_t i; };
		if(GetFixed32(src, &i))
		{
			v = f;
			src.remove_prefix(sizeof(float));
			return true;
		}
		return false;
	}
};


template<>
class K3Serializer<double> : public K3SerializerFixed64 {
public:
	static void PutValue(std::string& dst, double v)
	{
		union { double d; uint64_t l; };
		d = v;
		PutFixed64(dst, l);
	}
	static bool GetValue(std::string_view& src, double& v)
	{
		union { double d; uint64_t l; };
		if (GetFixed64(src, &l))
		{
			v = d;
			src.remove_prefix(sizeof(double));
			return true;
		}
		return false;
	}
};

template<>
class K3Serializer<std::string> : public K3SerializerVarint32
{
public:
	static void PutValue(std::string& dst, const std::string& v)
	{
		PutVarint32(dst, static_cast<uint32_t>(v.size()));
		dst.append(v.data(), v.size());
	}
	static bool GetValue(std::string_view& src, std::string& v)
	{
		uint32_t len;
		if (GetVarint32(src, &len) && src.size() >= len) {
			v.assign(src.data(), len);
			src.remove_prefix(len);
			return true;
		}
		else {
			return false;
		}
	}
};

template<typename T>
class K3Serializer<std::vector<T>> : public K3SerializerVarint32
{
public:
	static void PutValue(std::string& dst, const std::vector<T>& v)
	{
		PutVarint32(dst, static_cast<uint32_t>(v.size()));
		for (const auto& e : v)
		{
			K3Serializer<T>::PutValue(dst, e);
		}
	}
	static bool GetValue(std::string_view& src, std::vector<T>& v)
	{
		uint32_t vsize;
		if (!GetVarint32(src, &vsize))
		{
			return false;
		}
		for (uint32_t i = 0; i < vsize; ++i)
		{
			if (!K3Serializer<T>::GetValue(src, v.emplace_back()))
			{
				return false;
			}
		}
		return true;
	}
};


template<typename K, typename V>
class K3Serializer<std::unordered_map<K, V>> : public K3SerializerVarint32
{
public:
	static void PutValue(std::string& dst, const std::unordered_map<K,V>& v)
	{
		PutVarint32(dst, static_cast<uint32_t>(v.size()));
		for (const auto& kv : v)
		{
			K3Serializer<K>::PutValue(dst, kv.first);
			K3Serializer<V>::PutValue(dst, kv.second);
		}
	}
	static bool GetValue(std::string_view& src, std::unordered_map<K, V>& v)
	{
		uint32_t vsize;
		if (!GetVarint32(src, &vsize))
		{
			return false;
		}
		for (uint32_t i = 0; i < vsize; ++i)
		{
			std::pair<K, V> kv;
			if (!K3Serializer<K>::GetValue(src, kv.first))
			{
				return false;
			}
			if (!K3Serializer<V>::GetValue(src, kv.second))
			{
				return false;
			}
			v.insert(std::move(kv));
		}
		return true;
	}
};


template<typename T, typename = std::enable_if_t<std::is_class_v<T>>>
class K3SerializerClass
{
public:
	static void PutValue(std::string& dst, const T& obj)
	{
		if constexpr (!std::is_same_v<typename T::SuperClass, void>)
		{
			K3Serializer<typename T::SuperClass>::PutValue(dst, obj);
		}
		constexpr auto memberSize = std::tuple_size_v<decltype(T::kMetaClassMember)>;
		PutMember(dst, &obj, T::kMetaClassMember, std::make_index_sequence<memberSize>{});
	}
	static bool GetValue(std::string_view& src, T& obj)
	{
		if constexpr (!std::is_same_v<typename T::SuperClass, void>)
		{
			K3Serializer<typename T::SuperClass>::GetValue(src, obj);
		}
		constexpr auto memberSize = std::tuple_size_v<decltype(T::kMetaClassMember)>;
		return GetMember(src, &obj, T::kMetaClassMember, std::make_index_sequence<memberSize>{});
	}
protected:
	template <typename O, typename... Args, std::size_t... Idx>
	static void PutMember(std::string& dst, const O* o, const std::tuple<Args...>& t, std::index_sequence<Idx...>)
	{
		(K3Serializer<std::remove_const_t<std::remove_reference_t<decltype(o->*std::get<Idx>(t))>>>::PutValue(dst, o->*std::get<Idx>(t)), ...);
	}
	template <typename O, typename... Args, std::size_t... Idx>
	static bool GetMember(std::string_view& src, O* o, const std::tuple<Args...>& t, std::index_sequence<Idx...>)
	{
		return (K3Serializer<std::remove_const_t<std::remove_reference_t<decltype(o->*std::get<Idx>(t))>>>::GetValue(src, o->*std::get<Idx>(t)) && ...);
	}
};
