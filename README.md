# k3serializer

A fast, clean and small binary serializer based on modern c++17.

[中文介绍](https://github.com/OttoX/k3serializer/blob/master/README_CN.md)


[![Build Status](https://api.travis-ci.org/OttoX/k3serializer.svg?branch=master)](https://travis-ci.org/github/OttoX/k3serializer) [![Coverage Status](https://coveralls.io/repos/github/OttoX/k3serializer/badge.svg?branch=master)](https://coveralls.io/github/OttoX/k3serializer?branch=master)


### Example: base type
```c++
enum class ECountry
{
	US,
	CN,
	JP,
};
//add template specialization for enum class Type
template<>
class K3Serializer<ECountry> : public K3SerializerEnum<ECountry> {};

int main() {
	char c = 'a';
	int n = 0x12345678;
	float f = 3.14159f;
	ECountry e = ECountry::CN;

	std::string str;
	K3Serializer<char>::PutValue(str, c);
	K3Serializer<int>::PutValue(str, n);
	K3Serializer<float>::PutValue(str, f);
	K3Serializer<ECountry>::PutValue(str, e);

	char oc;
	int on;
	float of;
	ECountry oe;
	std::string_view input = str;
	assert(K3Serializer<char>::GetValue(input, oc) && oc == c);
	assert(K3Serializer<int>::GetValue(input, on) && on == n);
	assert(K3Serializer<float>::GetValue(input, of) && of == f);
	assert(K3Serializer<ECountry>::GetValue(input, oe) && oe == e);
}
```
### Example: stl container
```c++
int main() {
	std::vector<int> in1 = { 1,2,3,4,-3,-2,-1 };
	std::unordered_map<std::string, std::string> in2 = { {"first", "john"},  {"second", "bob"},  {"third", "jane"} };
	
	std::string str;
	K3Serializer<decltype(in1)>::PutValue(str, in1);
	K3Serializer<decltype(in2)>::PutValue(str, in2);

	std::vector<int> out1;
	std::unordered_map<std::string, std::string> out2;
	std::string_view input = str;
	assert(K3Serializer<decltype(out1)>::GetValue(input, out1) && in1 == out1);
	assert(K3Serializer<decltype(out2)>::GetValue(input, out2) && in2 == out2);

	return 0;
}
```
### Example: class
```c++
class Student
{
public:
	std::string name;
	std::vector<std::string> bookList;
    bool operator==(const Student& rhs)
	{
		return name == rhs.name && bookList == rhs.bookList;
	}
    //add annotation for class(like meta-data for refelction system)
    //we need to konw class-member and super-class info for serialize.
public:
	static constexpr inline auto kMetaClassMember = std::make_tuple(&Student::name, &Student::bookList);
	using SuperClass = void;
};
template<>
class K3Serializer<Student> : public K3SerializerClass<Student> {};

int main() {
	Student stu1;
	stu1.name = "bob";
	stu1.bookList = { "chinese", "math", "english", "physic" };
	
	std::string str;
	K3Serializer<decltype(stu1)>::PutValue(str, stu1); 

	Student stu2;
	std::string_view input = str;
	assert(K3Serializer<decltype(stu2)>::GetValue(input, stu2) && stu1 == stu2);
}
```
### Example: inherited-class
```c++
class K3Object
{
public:
	virtual ~K3Object() = default;

public:
	static constexpr inline auto kMetaClassMember = std::make_tuple();
	using SuperClass = void;
};
template<>
class K3Serializer<K3Object> : public K3SerializerClass<K3Object> {};

class Entity : public K3Object
{
public:
	int64_t entityId;

public:
	static constexpr inline auto kMetaClassMember = std::make_tuple(&Entity::entityId);
	using SuperClass = K3Object;
};
template<>
class K3Serializer<Entity> : public K3SerializerClass<Entity> {};

class Character : public Entity
{
public:
	std::string name;
	double money;
public:
	static constexpr inline auto kMetaClassMember = std::make_tuple(&Character::name, &Character::money);
	using SuperClass = Entity;
};
template<>
class K3Serializer<Character> : public K3SerializerClass<Character> {};

int main() {
	Character c1;
	c1.entityId = 0xA00000001;
	c1.name = "character_1";
	c1.money = 9999.9999;

	std::string str;
	K3Serializer<decltype(c1)>::PutValue(str, c1); 

	Character c2;
	std::string_view input = str;
	bool result = K3Serializer<decltype(c2)>::GetValue(input, c2);
	assert(result);
}
```