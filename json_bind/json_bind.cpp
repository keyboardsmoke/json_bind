// json_bind.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include "Bind.h"

std::string test_json =
R"({
	"name" : "hello_world",
	"subsection" : 
	{
		"ss1" : { "ss2" : { "ss3" : { "checkit" : true } } },
		
		"test_int" : -12,
		"test_uint" : 4294967295,
		"test_string" : "foobar",
		"test_double" : 3.19,
		"test_array" : [1, 2, 3, 4],
		"test_object_array" : [{"set" : true}, {"set" : false}],
		"test_bool" : true
	}
})";

class ArrayObject : public BoundObject
{
public:
	ArrayObject(BoundElement* parent, std::string name) : BoundObject(parent, name) {}

	BoundBoolean set = BoundBoolean(this, "set");
};

class BoundSS3 : public BoundObject
{
public:
	BoundSS3(BoundElement* parent, std::string name) : BoundObject(parent, name) {}

	BoundBoolean checkit = BoundBoolean(this, "checkit");
};

class BoundSS2 : public BoundObject
{
public:
	BoundSS2(BoundElement* parent, std::string name) : BoundObject(parent, name) {}

	BoundSS3 ss3 = BoundSS3(this, "ss3");
};

class BoundSS1 : public BoundObject
{
public:
	BoundSS1(BoundElement* parent, std::string name) : BoundObject(parent, name) {}

	BoundSS2 ss2 = BoundSS2(this, "ss2");
};

class BoundSubSection : public BoundObject
{
public:
	BoundSubSection(BoundElement* parent, std::string name) : BoundObject(parent, name) {}

	BoundSS1 ss1 = BoundSS1(this, "ss1");
	BoundInteger test_int = BoundInteger(this, "test_int");
	BoundUnsignedInteger test_uint = BoundUnsignedInteger(this, "test_uint");
	BoundString test_string = BoundString(this, "test_string");
	BoundDouble test_double = BoundDouble(this, "test_double");
	BoundArray<BoundInteger> test_array = BoundArray<BoundInteger>(this, "test_array");
	BoundArray<ArrayObject> test_object_array = BoundArray<ArrayObject>(this, "test_object_array");
	BoundBoolean test_bool = BoundBoolean(this, "test_bool");
};

class BoundRoot : public BoundObject
{
public:
	BoundRoot(Json::Value root, bool requireAllFields) : BoundObject(root, requireAllFields) {}

	BoundString name = BoundString(this, "name");
	BoundSubSection subsection = BoundSubSection(this, "subsection");
};

static std::string LoadFile(const std::string& filename)
{
	std::ifstream ifs(filename);

	if (ifs.bad())
	{
		throw std::exception("File could not be read.");
	}

	return std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
}

int main()
{
	Json::CharReaderBuilder builder;

	std::unique_ptr<Json::CharReader> const reader(builder.newCharReader());

	Json::Value root;
	std::string errs;
	if (!reader->parse(test_json.c_str(), test_json.c_str() + test_json.size(), &root, &errs))
	{
		std::cout << "Unable to read JSON" << std::endl;
		std::cout << errs << std::endl;
		return 1;
	}

	BoundRoot root_data(root, true);

	try
	{
		root_data.Initialize();
	}
	catch (BoundException& e)
	{
		std::cout << "Unable to initialize root_data (" << e.what() << ")" << std::endl;
		return 1;
	}

	std::cout << "Name: " << root_data.name.GetValue() << std::endl;

	std::cout << "SubSection::test_int = " << root_data.subsection.test_int.GetValue() << std::endl;
	std::cout << "SubSection::test_uint = " << root_data.subsection.test_uint.GetValue() << std::endl;
	std::cout << "SubSection::test_string = " << root_data.subsection.test_string.GetValue() << std::endl;
	std::cout << "SubSection::test_double = " << root_data.subsection.test_double.GetValue() << std::endl;

	if (root_data.subsection.test_bool.IsDefined())
		std::cout << "SubSection::test_bool = " << root_data.subsection.test_bool.GetValue() << std::endl;
	else
		std::cout << "SubSection::test_bool = <undefined>" << std::endl;

	// Test the array
	auto array = root_data.subsection.test_array.GetValue();
	for (auto& val : array)
	{
		std::cout << "Array value (" << val->GetValue() << ")" << std::endl;
	}

	auto object_array = root_data.subsection.test_object_array.GetValue();
	for (auto& val : object_array)
	{
		std::cout << "Object set = " << val->set.GetValue() << std::endl;
	}

	BoundBoolean& checkit = root_data.subsection.ss1.ss2.ss3.checkit;

	std::cout << "Checkit = " << checkit.GetValue() << std::endl;

	return 0;
}