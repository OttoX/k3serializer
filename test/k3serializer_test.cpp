#include "../k3serializer.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <limits>
#include <algorithm>

TEST_CASE( "test lenth", "[VarintLength]" ) {
	int in1 = 255;
	uint64_t in2 = static_cast<uint32_t>(-1);
	uint64_t in3 = 0x123456789ABCDEF;
	uint64_t in4 = std::numeric_limits<uint64_t>::max();
	REQUIRE((K3SerializerBase::VarintLength(in1) == 2));
	REQUIRE((K3SerializerBase::VarintLength(in2) == 5));
	REQUIRE((K3SerializerBase::VarintLength(in3) == 9));
	REQUIRE((K3SerializerBase::VarintLength(in4) == 10));
}

TEST_CASE( "testing byte", "[char, int8_t, uint8_t]" ) {
    std::string str;
    char inChar = 'a';
    int8_t inInt8 = -123;
    uint8_t inUInt8 = 233;
    K3Serializer<char>::PutValue(str, inChar);
    K3Serializer<int8_t>::PutValue(str, inInt8);
    K3Serializer<uint8_t>::PutValue(str, inUInt8);
    std::string_view input = str;
    char outChar;
    int8_t outInt8;
    uint8_t outUInt8;
    K3Serializer<char>::GetValue(input, outChar);
    K3Serializer<int8_t>::GetValue(input, outInt8);
    K3Serializer<uint8_t>::GetValue(input, outUInt8);
    REQUIRE((inChar == outChar && inInt8 == outInt8 && inUInt8 == outUInt8));
}

TEST_CASE( "testing varint32", "[int16_t, uint16_t, int32_t, uint32_t]" ) {
    std::string str;
    int16_t inInt16 = -13579;
    uint16_t inUInt16 = 0xFFFF;
	int32_t inInt32 = 0xABCDEF;
    uint32_t inUInt32 = -1;
    K3Serializer<int16_t>::PutValue(str, inInt16);
    K3Serializer<uint16_t>::PutValue(str, inUInt16);
    K3Serializer<int32_t>::PutValue(str, inInt32);
    K3Serializer<uint32_t>::PutValue(str, inUInt32);
    std::string_view input = str;
    int16_t outInt16;
    uint16_t outUInt16;
    int32_t outInt32;
    uint32_t outUInt32;
    K3Serializer<int16_t>::GetValue(input, outInt16);
    K3Serializer<uint16_t>::GetValue(input, outUInt16);
    K3Serializer<int32_t>::GetValue(input, outInt32);
    K3Serializer<uint32_t>::GetValue(input, outUInt32);
    REQUIRE((inInt16 == outInt16 && inUInt16 == outUInt16
        && inInt32 == outInt32 && inUInt32 == outUInt32));
}

TEST_CASE( "testing varint64", "[int64_t, uint64_t]" ) {
    std::string str;
    int64_t in1Int64 = std::numeric_limits<int64_t>::min();
    uint64_t in1UInt64 = std::numeric_limits<uint64_t>::min();
    int64_t in2Int64 = std::numeric_limits<int64_t>::max();
    uint64_t in2UInt64 = std::numeric_limits<uint64_t>::max();
    int64_t in3Int64 = 0;
    uint64_t in3UInt64 = 0xFEDCBA9876543219;
    K3Serializer<int64_t>::PutValue(str, in1Int64);
    K3Serializer<uint64_t>::PutValue(str, in1UInt64);
    K3Serializer<int64_t>::PutValue(str, in2Int64);
    K3Serializer<uint64_t>::PutValue(str, in2UInt64);
    K3Serializer<int64_t>::PutValue(str, in3Int64);
    K3Serializer<uint64_t>::PutValue(str, in3UInt64);
    std::string_view input = str;
    int64_t out1Int64;
    uint64_t out1UInt64;
    int64_t out2Int64;
    uint64_t out2UInt64;
    int64_t out3Int64;
    uint64_t out3UInt64;
    K3Serializer<int64_t>::GetValue(input, out1Int64);
    K3Serializer<uint64_t>::GetValue(input, out1UInt64);
    K3Serializer<int64_t>::GetValue(input, out2Int64);
    K3Serializer<uint64_t>::GetValue(input, out2UInt64);
    K3Serializer<int64_t>::GetValue(input, out3Int64);
    K3Serializer<uint64_t>::GetValue(input, out3UInt64);
    REQUIRE((in1Int64 == out1Int64 && in1UInt64 == out1UInt64 && in2Int64 == out2Int64 
        && in2UInt64 == out2UInt64 && in3Int64 == out3Int64 && in3UInt64 == out3UInt64));
}

template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
    almost_equal(T x, T y)
{
    return std::fabs(x-y) <= std::numeric_limits<T>::epsilon();
}

TEST_CASE( "testing fixed", "[float, double]" ) {
    std::string str;
    float in1 = std::numeric_limits<float>::min();
    double in2 = std::numeric_limits<double>::min();
    float in3 = std::numeric_limits<float>::max();
    double in4 = std::numeric_limits<double>::max();
    float in5 = 6789.12345;
    double in6 = 9876543210.0123456789;
    K3Serializer<float>::PutValue(str, in1);
    K3Serializer<double>::PutValue(str, in2);
    K3Serializer<float>::PutValue(str, in3);
    K3Serializer<double>::PutValue(str, in4);
    K3Serializer<float>::PutValue(str, in5);
    K3Serializer<double>::PutValue(str, in6);
    std::string_view input = str;
    float out1;
    double out2;
    float out3;
    double out4;
    float out5;
    double out6;
    K3Serializer<float>::GetValue(input, out1);
    K3Serializer<double>::GetValue(input, out2);
    K3Serializer<float>::GetValue(input, out3);
    K3Serializer<double>::GetValue(input, out4);
    K3Serializer<float>::GetValue(input, out5);
    K3Serializer<double>::GetValue(input, out6);
    REQUIRE((almost_equal(in1, out1) && almost_equal(in2, out2) && almost_equal(in3, out3) 
        && almost_equal(in4, out4)  && almost_equal(in5, out5)  && almost_equal(in6, out6)));
}

TEST_CASE( "testing string", "[std::string]" ) {
    std::string str;
    std::string in1 = u8"hello, world.你好。こんにちは。";
    std::string in2 = "";
    K3Serializer<std::string>::PutValue(str, in1);
    K3Serializer<std::string>::PutValue(str, in2);
    std::string_view input = str;
    std::string out1;
    std::string out2;
    K3Serializer<std::string>::GetValue(input, out1);
    K3Serializer<std::string>::GetValue(input, out2);
	REQUIRE((in1==out1 && in2==out2));
}

enum EDay : uint8_t
{
    Monday,
    Tuesday,
    Wednesday, 
    Thursday,
    Friday,
    Saturday,
    Sunday,
};
template<>
class K3Serializer<EDay> : public K3SerializerEnum<EDay> {};

enum class ECountry
{
	US,
	China,
	Japan,
};
template<>
class K3Serializer<ECountry> : public K3SerializerEnum<ECountry> {};


TEST_CASE( "testing enum", "[enum, enum class]" ) {
    std::string str;
    EDay in1 = EDay::Wednesday;
    ECountry in2 = ECountry::Japan;
    K3Serializer<EDay>::PutValue(str, in1);
    K3Serializer<ECountry>::PutValue(str, in2);
    std::string_view input = str;
    EDay out1;
    ECountry out2;
    K3Serializer<EDay>::GetValue(input, out1);
    K3Serializer<ECountry>::GetValue(input, out2);
	REQUIRE((in1==out1 && in2==out2));
}

TEST_CASE( "testing vector", "[vector]" ) {
    std::string str;
    std::vector<int> in1 = {1,2,3,4,-3,-2,-1};
    std::vector<std::string> in2 = {"", "hello, world.", "你好！", ""};
    K3Serializer<std::vector<int>>::PutValue(str, in1);
    K3Serializer<std::vector<std::string>>::PutValue(str, in2);
    std::string_view input = str;
    std::vector<int> out1;
    std::vector<std::string> out2;
    K3Serializer<decltype(out1)>::GetValue(input, out1);
    K3Serializer<decltype(out2)>::GetValue(input, out2);
	REQUIRE((in1==out1 && in2==out2));
}

TEST_CASE( "testing unordered_map", "[map]" ) {
    std::string str;
    std::unordered_map<int, std::string> in1 = {{1, "hello, world."}, {202031, "你好"}, {1900, "問天地好在"}, {-9810, ""}};
    std::unordered_map<std::string, int> in2 = {{"",0}, {"hello, world.", 111}, {"你好！", -2222}};
    K3Serializer<decltype(in1)>::PutValue(str, in1);
    K3Serializer<decltype(in2)>::PutValue(str, in2);
    std::string_view input = str;
    std::unordered_map<int, std::string> out1;
    std::unordered_map<std::string, int> out2;
    K3Serializer<decltype(out1)>::GetValue(input, out1);
    K3Serializer<decltype(out2)>::GetValue(input, out2);
	REQUIRE((in1==out1 && in2==out2));
}

class K3Object
{
public:
	virtual ~K3Object() = default;

public: //<= annotation
	static constexpr inline auto kMetaClassMember = std::make_tuple();
	using SuperClass = void;
};
template<>
class K3Serializer<K3Object> : public K3SerializerClass<K3Object> {};

class Actor : public K3Object
{
public:
	ECountry country;

public:
	static constexpr inline auto kMetaClassMember = std::make_tuple(&Actor::country);
	using SuperClass = K3Object;
};
template<>
class K3Serializer<Actor> : public K3SerializerClass<Actor> {};

class Person : public Actor
{
public:
	std::string name;
	int age;
	double money;

    bool operator==(const Person& rhs)
    {
        return name == rhs.name && age == rhs.age && money == rhs.money && country == rhs.country;
    }
	friend bool operator==(const Person& lhs, const Person& rhs)
	{
		return lhs.name == rhs.name && lhs.age == rhs.age && lhs.money == rhs.money && lhs.country == rhs.country;
	}
public:
	static constexpr inline auto kMetaClassMember = std::make_tuple(&Person::name, &Person::age, &Person::money);
	using SuperClass = Actor;
};
template<>
class K3Serializer<Person> : public K3SerializerClass<Person> {};

TEST_CASE( "testing object", "[class]" ) {
	Person p1;
	p1.country = ECountry::US;
	p1.name = "Jim";
	p1.age = 22;
	p1.money = 12.345;

    std::string str;
	K3Serializer<Person>::PutValue(str, p1);

	std::string_view input = str;
	Person p2;
	K3Serializer<Person>::GetValue(input, p2);

    REQUIRE((p1==p2));
}

class Student
{
public:
	std::string name;
	std::vector<std::string> bookList;
	std::unordered_map<std::string, Person> friends;

    bool operator==(const Student& rhs)
    {
        return name == rhs.name && bookList == rhs.bookList && friends == rhs.friends;
    }
public:
	static constexpr inline auto kMetaClassMember = std::make_tuple(&Student::name, &Student::bookList, &Student::friends);
	using SuperClass = void;
};
template<>
class K3Serializer<Student> : public K3SerializerClass<Student> {};

TEST_CASE( "testing complicated object", "[inherited-class]" ) {
	Person p1;
	p1.country = ECountry::US;
	p1.name = "Jim";
	p1.age = 22;
	p1.money = 12.345;
    Person p2;
	p2.country = ECountry::China;
	p2.name = "张三";
	p2.age = 43;
	p2.money = -9999.345;
	Person p3;
	p3.country = ECountry::Japan;
	p3.name = "田中さん";
	p3.age = 30;
	p3.money = 22.22222222;

	Student stu1;
	stu1.name = "bob";
	stu1.bookList = { "chinese", "math", "english", "physic" };
	stu1.friends = { {p1.name, p1}, {p2.name, p2}, {p3.name, p3} };

    std::string str;
	K3Serializer<Student>::PutValue(str, stu1);

	std::string_view input = str;
	Student stu2;
	K3Serializer<Student>::GetValue(input, stu2);

    REQUIRE((stu1==stu2));
}

TEST_CASE( "testing error branch", "[error]" ) {
	std::string_view input;
    char c = 'a';
    uint32_t u;
    int64_t l;
    REQUIRE((K3Serializer<char>::GetValue(input, c) == false));
    REQUIRE((K3Serializer<uint32_t>::GetValue(input, u) == false));
    REQUIRE((K3Serializer<int64_t>::GetValue(input, l) == false));
    std::string str;
    K3Serializer<char>::PutValue(str, c);
    input = str;
    float f = 12345678.9999;
    REQUIRE((K3Serializer<float>::GetValue(input, f) == false));
    K3Serializer<float>::PutValue(str, f);
    double d = std::numeric_limits<double>::max();;
    REQUIRE((K3Serializer<double>::GetValue(input, d) == false));
}