/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "tests/StringPartTest.h"

#include <string>

#include "data/String.h"


namespace ham {
namespace tests {


using data::String;


static int
sign(int number)
{
	return number == 0 ? 0 : (number < 0 ? - 1 : 1);
}


#define STRING_EQUAL(actual, expected)							\
	HAM_TEST_EQUAL(actual.ToStlString(), std::string(expected))	\
	HAM_TEST_EQUAL(actual.Length(), strlen(expected))			\
	HAM_TEST_EQUAL(actual.IsEmpty(), strlen(expected) == 0)

#define STRING_COMPARE_WORK(string1, string2, expected)				\
	HAM_TEST_EQUAL(sign(string1.CompareWith(string2)), expected)	\
	HAM_TEST_EQUAL(string1 == string2, expected == 0)				\
	HAM_TEST_EQUAL(string1 != string2, expected != 0)				\
	HAM_TEST_EQUAL(string1 < string2, expected < 0)					\
	HAM_TEST_EQUAL(string1 > string2, expected > 0)					\
	HAM_TEST_EQUAL(string1 <= string2, expected <= 0)				\
	HAM_TEST_EQUAL(string1 >= string2, expected >= 0)

#define STRING_COMPARE(string1, string2, expected)				\
	STRING_COMPARE_WORK(string1, string2, expected)				\
	STRING_COMPARE_WORK(string2, string1, -expected)


void
ham::tests::StringPartTest::Constructor()
{
	// default constructor
	{
		StringPart string;
		STRING_EQUAL(string, "")
	}

	// (const char*) constructor
	{
		StringPart string("");
		STRING_EQUAL(string, "")
	}

	{
		StringPart string("foo");
		STRING_EQUAL(string, "foo")
	}

	// (const char*, const char*) constructor
	{
		const char* testString = "foobar";
		StringPart string(testString, testString);
		STRING_EQUAL(string, "")
	}

	{
		const char* testString = "foobar";
		StringPart string("foobar", testString + 5);
		STRING_EQUAL(string, "fooba")
	}

	// (const char*, size_t) constructor
	{
		StringPart string("foobar", (size_t)0);
		STRING_EQUAL(string, "")
	}

	{
		StringPart string("foobar", (size_t)5);
		STRING_EQUAL(string, "fooba")
	}

	// copy constructor
	{
		StringPart string1;
		STRING_EQUAL(string1, "")
		StringPart string2(string1);
		STRING_EQUAL(string2, "")
	}

	{
		StringPart string1("foobar");
		STRING_EQUAL(string1, "foobar")
		StringPart string2(string1);
		STRING_EQUAL(string2, "foobar")
	}
}


void
StringPartTest::Comparison()
{
	struct TestData {
		const char*	string1;
		const char*	string2;
		int			compare;
	};

	const TestData testData[] = {
		{ "",		"",			0 },
		{ "foo",	"",			1 },
		{ "foo",	"foo",		0 },
		{ "foobar",	"foo",		1 },
		{ "foo",	"bar",		1 },
		{ "foo",	"barfoo",	1 },
		{ "foobar",	"bar",		1 }
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		StringPart string1(testData[i].string1);
		StringPart string2(testData[i].string2);
		STRING_COMPARE(string1, string2, testData[i].compare)
	}

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		std::string testString1 = std::string(testData[i].string1) + "bbb";
		std::string testString2 = std::string(testData[i].string2) + "aa";
		StringPart string1(testString1.c_str(), strlen(testData[i].string1));
		StringPart string2(testString2.c_str(), strlen(testData[i].string2));
		STRING_COMPARE(string1, string2, testData[i].compare)
	}
}


void
StringPartTest::Assignment()
{
	struct TestData {
		const char*	string1;
		const char*	string2;
	};

	const TestData testData[] = {
		{ "",		"" },
		{ "foo",	"" },
		{ "foo",	"foo" },
		{ "foo",	"bar" },
		{ "foobar",	"foo" }
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		const char* testString1 = testData[i].string1;
		const char* testString2 = testData[i].string2;
		for (int k = 0; k < 2; k++) {
			StringPart string1(testString1);
			StringPart string2(testString2);
			STRING_EQUAL(string1, testString1)
			STRING_EQUAL(string2, testString2)

			string1 = string2;
			STRING_EQUAL(string1, testString2)
			STRING_EQUAL(string2, testString2)

			std::swap(testString1, testString2);
		}
	}

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		const char* testString1 = testData[i].string1;
		const char* testString2 = testData[i].string2;
		for (int k = 0; k < 2; k++) {
			std::string stlTestString1 = std::string(testString1) + "bbb";
			std::string stlTestString2 = std::string(testString2) + "aa";
			StringPart string1(stlTestString1.c_str(), strlen(testString1));
			StringPart string2(stlTestString2.c_str(), strlen(testString2));
			STRING_EQUAL(string1, testString1)
			STRING_EQUAL(string2, testString2)

			string1 = string2;
			STRING_EQUAL(string1, testString2)
			STRING_EQUAL(string2, testString2)

			std::swap(testString1, testString2);
		}
	}
}


} // namespace tests
} // namespace ham
