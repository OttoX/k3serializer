#include "k3serializer.h"

int K3SerializerBase::VarintLength(uint64_t v)
{
	int len = 1;
	while (v >= 128) {
		v >>= 7;
		len++;
	}
	return len;
}
void K3SerializerBase::EncodeFixed32(char* buf, uint32_t v)
{
	if constexpr (port::kLittleEndian) {
		memcpy(buf, &v, sizeof(v));
	}
	else {
		buf[0] = v & 0xff;
		buf[1] = (v >> 8) & 0xff;
		buf[2] = (v >> 16) & 0xff;
		buf[3] = (v >> 24) & 0xff;
	}
}
char* K3SerializerBase::EncodeVarint32(char* dst, uint32_t v) {
	// Operate on characters as unsigneds
	unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
	static const int B = 128;
	if (v < (1 << 7)) {
		*(ptr++) = v;
	}
	else if (v < (1 << 14)) {
		*(ptr++) = v | B;
		*(ptr++) = v >> 7;
	}
	else if (v < (1 << 21)) {
		*(ptr++) = v | B;
		*(ptr++) = (v >> 7) | B;
		*(ptr++) = v >> 14;
	}
	else if (v < (1 << 28)) {
		*(ptr++) = v | B;
		*(ptr++) = (v >> 7) | B;
		*(ptr++) = (v >> 14) | B;
		*(ptr++) = v >> 21;
	}
	else {
		*(ptr++) = v | B;
		*(ptr++) = (v >> 7) | B;
		*(ptr++) = (v >> 14) | B;
		*(ptr++) = (v >> 21) | B;
		*(ptr++) = v >> 28;
	}
	return reinterpret_cast<char*>(ptr);
}
const char* K3SerializerBase::GetVarint32Ptr(const char* p, const char* limit, uint32_t* v)
{
	if (p < limit) {
		uint32_t result = *(reinterpret_cast<const unsigned char*>(p));
		if ((result & 128) == 0) {
			*v = result;
			return p + 1;
		}
	}
	return GetVarint32PtrFallback(p, limit, v);
}
const char* K3SerializerBase::GetVarint32PtrFallback(const char* p, const char* limit, uint32_t* v) {
	uint32_t result = 0;
	for (uint32_t shift = 0; shift <= 28 && p < limit; shift += 7) {
		uint32_t byte = *(reinterpret_cast<const unsigned char*>(p));
		p++;
		if (byte & 128) {
			// More bytes are present
			result |= ((byte & 127) << shift);
		}
		else {
			result |= (byte << shift);
			*v = result;
			return reinterpret_cast<const char*>(p);
		}
	}
	return nullptr;
}
char* K3SerializerBase::EncodeVarint64(char* dst, uint64_t v) {
	static const int B = 128;
	unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
	while (v >= B) {
		*(ptr++) = (v & (B - 1)) | B;
		v >>= 7;
	}
	*(ptr++) = static_cast<unsigned char>(v);
	return reinterpret_cast<char*>(ptr);
}
void K3SerializerBase::EncodeFixed64(char* buf, uint64_t v)
{
	if (port::kLittleEndian) {
		memcpy(buf, &v, sizeof(v));
	}
	else {
		buf[0] = v & 0xff;
		buf[1] = (v >> 8) & 0xff;
		buf[2] = (v >> 16) & 0xff;
		buf[3] = (v >> 24) & 0xff;
		buf[4] = (v >> 32) & 0xff;
		buf[5] = (v >> 40) & 0xff;
		buf[6] = (v >> 48) & 0xff;
		buf[7] = (v >> 56) & 0xff;
	}
}
const char* K3SerializerBase::GetVarint64Ptr(const char* p, const char* limit, uint64_t* v)
{
	uint64_t result = 0;
	for (uint32_t shift = 0; shift <= 63 && p < limit; shift += 7) {
		uint64_t byte = *(reinterpret_cast<const unsigned char*>(p));
		p++;
		if (byte & 128) {
			// More bytes are present
			result |= ((byte & 127) << shift);
		}
		else {
			result |= (byte << shift);
			*v = result;
			return reinterpret_cast<const char*>(p);
		}
	}
	return nullptr;
}

bool K3SerializerByte::GetByte(std::string_view& input, uint8_t* v)
{
	if (input.size() < sizeof(uint8_t))
	{
		return false;
	}
	*v = static_cast<uint8_t>(input.front());
	input.remove_prefix(sizeof(uint8_t));
	return true;
}

void K3SerializerVarint32::PutVarint32(std::string& dst, uint32_t v)
{
	char buf[5];
	char* ptr = EncodeVarint32(buf, v);
	dst.append(buf, ptr - buf);
}
bool K3SerializerVarint32::GetVarint32(std::string_view& input, uint32_t* v)
{
	const char* p = input.data();
	const char* limit = p + input.size();
	const char* q = GetVarint32Ptr(p, limit, v);
	if (q == nullptr) {
		return false;
	}
	else {
		input = std::string_view(q, limit - q);
		return true;
	}
}

void K3SerializerFixed32::PutFixed32(std::string& dst, uint32_t v)
{
	char buf[sizeof(v)];
	EncodeFixed32(buf, v);
	dst.append(buf, sizeof(buf));
}
bool K3SerializerFixed32::GetFixed32(std::string_view& input, uint32_t* v)
{
	if (input.size() < sizeof(uint32_t))
	{
		return false;
	}
	*v = DecodeFixed32(input.data());
	return true;
}

void K3SerializerVarint64::PutVarint64(std::string& dst, uint64_t v)
{
	char buf[10];
	char* ptr = EncodeVarint64(buf, v);
	dst.append(buf, ptr - buf);
}
bool K3SerializerVarint64::GetVarint64(std::string_view& input, uint64_t* v)
{
	const char* p = input.data();
	const char* limit = p + input.size();
	const char* q = GetVarint64Ptr(p, limit, v);
	if (q == nullptr) {
		return false;
	}
	else {
		input = std::string_view(q, limit - q);
		return true;
	}
}

void K3SerializerFixed64::PutFixed64(std::string& dst, uint64_t v)
{
	char buf[sizeof(v)];
	EncodeFixed64(buf, v);
	dst.append(buf, sizeof(buf));
}
bool K3SerializerFixed64::GetFixed64(std::string_view& input, uint64_t* v)
{
	if (input.size() < sizeof(uint64_t))
	{
		return false;
	}
	*v = DecodeFixed64(input.data());
	return true;
}