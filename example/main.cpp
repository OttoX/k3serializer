#include "../k3serializer.h"
#include <iostream>

enum class ECountry
{
	US,
	China,
	Japan,
};
template<>
class K3Serializer<ECountry> : public K3SerializerEnum<ECountry> {};

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

public:
	static constexpr inline auto kMetaClassMember = std::make_tuple(&Person::name, &Person::age, &Person::money);
	using SuperClass = Actor;
};
template<>
class K3Serializer<Person> : public K3SerializerClass<Person> {};

class Student
{
public:
	std::string name;
	std::vector<std::string> bookList;
	std::unordered_map<std::string, Person> friends;
public:
	static constexpr inline auto kMetaClassMember = std::make_tuple(&Student::name, &Student::bookList, &Student::friends);
	using SuperClass = void;
};
template<>
class K3Serializer<Student> : public K3SerializerClass<Student> {};

#include <sstream>
int main()
{
	Person p1;
	p1.country = ECountry::US;
	p1.name = "Jim";
	p1.age = 22;
	p1.money = 12.345;

	std::string output;
	K3Serializer<Person>::PutValue(output, p1);

	std::string_view input = output;
	Person p2;
	K3Serializer<Person>::GetValue(input, p2);

	std::cout << "after deserialize: p2.name=" << p2.name << " p2.country=" << static_cast<int>(p2.country)
		<< " p2.age=" << p2.age << " p2.money=" << p2.money << std::endl;


	Person p3;
	p3.country = ECountry::Japan;
	p3.name = "Mary";
	p3.age = 20;
	p3.money = 22.22;

	output.clear();
	Student stu1;
	stu1.name = "bob";
	stu1.bookList = { "chinese", "math", "english", "physic" };
	stu1.friends = { {p1.name, p1}, {p3.name, p3} };
	K3Serializer<Student>::PutValue(output, stu1);

	input = output;
	Student stu2;
	K3Serializer<Student>::GetValue(input, stu2);


	std::ostringstream oss;
	oss << "after deserialize: stu2.name=" << stu2.name;
	oss << " booklist={";
	for (const auto& book : stu2.bookList)
	{
		oss << book << ",";
	}
	oss.seekp(-1, std::ios_base::end);
	oss << "} ";
	for (const auto& e : stu2.friends)
	{
		const Person& p = e.second;
		oss << "{name=" << e.first << ", country=" << static_cast<int>(p.country)
			<< " age=" << p.age << " money=" << p.money << "} ";
	}
	std::cout << oss.str() << std::endl;

	return 0;
}