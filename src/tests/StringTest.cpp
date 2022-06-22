/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "tests/StringTest.hpp"

#include <string>

#include "data/String.hpp"

namespace ham::tests
{

using data::String;

static int
sign(int number)
{
	return number == 0 ? 0 : (number < 0 ? -1 : 1);
}

#define STRING_EQUAL(actual, expected)                                         \
	HAM_TEST_EQUAL(actual.ToCString(), std::string(expected))                  \
	HAM_TEST_EQUAL(actual.ToStlString(), std::string(expected))                \
	HAM_TEST_EQUAL(actual.Length(), strlen(expected))                          \
	HAM_TEST_EQUAL(actual.IsEmpty(), strlen(expected) == 0)

#define STRING_COMPARE_WORK(string1, string2, expected)                        \
	HAM_TEST_EQUAL(sign(string1.CompareWith(string2)), expected)               \
	HAM_TEST_EQUAL(string1 == string2, expected == 0)                          \
	HAM_TEST_EQUAL(string1 != string2, expected != 0)                          \
	HAM_TEST_EQUAL(string1 < string2, expected < 0)                            \
	HAM_TEST_EQUAL(string1 > string2, expected > 0)                            \
	HAM_TEST_EQUAL(string1 <= string2, expected <= 0)                          \
	HAM_TEST_EQUAL(string1 >= string2, expected >= 0)

#define STRING_COMPARE(string1, string2, expected)                             \
	STRING_COMPARE_WORK(string1, string2, expected)                            \
	STRING_COMPARE_WORK(string2, string1, -expected)

void
StringTest::Constructor()
{
	// default constructor
	{
		String string;
		STRING_EQUAL(string, "")
	}

	// (const char*) constructor
	{
		String string("");
		STRING_EQUAL(string, "")
	}

	{
		String string("foo");
		STRING_EQUAL(string, "foo")
	}

	// (const char*, size_t) constructor
	{
		String string("foobar", 0);
		STRING_EQUAL(string, "")
	}

	{
		String string("foobar", 5);
		STRING_EQUAL(string, "fooba")
	}

	// (const StringPart&) constructor
	{
		StringPart stringPart;
		String string(stringPart);
		STRING_EQUAL(string, "")
	}

	{
		StringPart stringPart("foo", (size_t)0);
		String string(stringPart);
		STRING_EQUAL(string, "")
	}

	{
		StringPart stringPart("foo");
		String string(stringPart);
		STRING_EQUAL(string, "foo")
	}

	{
		StringPart stringPart("foobar", (size_t)4);
		String string(stringPart);
		STRING_EQUAL(string, "foob")
	}

	// copy constructor
	{
		String string1;
		STRING_EQUAL(string1, "")
		String string2(string1);
		STRING_EQUAL(string2, "")
	}

	{
		String string1("foobar");
		STRING_EQUAL(string1, "foobar")
		String string2(string1);
		STRING_EQUAL(string2, "foobar")
	}
}

void
StringTest::CastOperator()
{
	{
		String string;
		StringPart stringPart(string);
		HAM_TEST_EQUAL(stringPart.Start(), string.ToCString())
		HAM_TEST_EQUAL(stringPart.Length(), string.Length())
	}

	{
		String string("foobar");
		StringPart stringPart(string);
		HAM_TEST_EQUAL(stringPart.Start(), string.ToCString())
		HAM_TEST_EQUAL(stringPart.Length(), string.Length())
	}
}

void
StringTest::Comparison()
{
	struct TestData {
		const char* string1;
		const char* string2;
		int compare;
	};

	const TestData testData[] = {{"", "", 0},
								 {"foo", "", 1},
								 {"foo", "foo", 0},
								 {"foobar", "foo", 1},
								 {"foo", "bar", 1},
								 {"foo", "barfoo", 1},
								 {"foobar", "bar", 1}};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		String string1(testData[i].string1);
		String string2(testData[i].string2);
		STRING_COMPARE(string1, string2, testData[i].compare)
	}
}

void
StringTest::Assignment()
{
	struct TestData {
		const char* string1;
		const char* string2;
	};

	const TestData testData[] = {{"", ""},
								 {"foo", ""},
								 {"foo", "foo"},
								 {"foo", "bar"},
								 {"foobar", "foo"}};

	// =(const String&)
	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		const char* testString1 = testData[i].string1;
		const char* testString2 = testData[i].string2;
		for (int k = 0; k < 2; k++) {
			String string1(testString1);
			String string2(testString2);
			STRING_EQUAL(string1, testString1)
			STRING_EQUAL(string2, testString2)

			string1 = string2;
			STRING_EQUAL(string1, testString2)
			STRING_EQUAL(string2, testString2)

			std::swap(testString1, testString2);
		}
	}

	// =(const StringPart&)
	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		const char* testString1 = testData[i].string1;
		const char* testString2 = testData[i].string2;
		for (int k = 0; k < 2; k++) {
			String string1(testString1);
			std::string stlTestString2 = std::string(testString2) + "xxx";
			StringPart string2(stlTestString2.c_str(), strlen(testString2));
			STRING_EQUAL(string1, testString1)
			HAM_TEST_EQUAL(string2.ToStlString(), testString2)

			string1 = string2;
			STRING_EQUAL(string1, testString2)
			HAM_TEST_EQUAL(string2.ToStlString(), testString2)

			std::swap(testString1, testString2);
		}
	}
}

void
StringTest::Concatenation()
{
	struct TestData {
		const char* string1;
		const char* string2;
	};

	const TestData testData[] = {{"", ""},
								 {"foo", ""},
								 {"foo", "foo"},
								 {"foo", "bar"},
								 {"foobar", "foo"}};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		const char* testString1 = testData[i].string1;
		const char* testString2 = testData[i].string2;
		for (int k = 0; k < 2; k++) {
			std::string stlTestString1 = std::string(testString1) + "xxx";
			std::string stlTestString2 = std::string(testString2) + "zzz";
			std::string testString3 = std::string(testString1) + testString2;

			// String + String
			{
				String string1(testString1);
				String string2(testString2);
				STRING_EQUAL(string1, testString1)
				STRING_EQUAL(string2, testString2)

				String string3 = string1 + string2;

				STRING_EQUAL(string1, testString1)
				STRING_EQUAL(string2, testString2)
				STRING_EQUAL(string3, testString3.c_str())
			}

			// String + StringPart
			{
				String string1(testString1);
				StringPart string2(stlTestString2.c_str(), strlen(testString2));
				STRING_EQUAL(string1, testString1)
				HAM_TEST_EQUAL(string2.ToStlString(), testString2)

				String string3 = string1 + string2;

				STRING_EQUAL(string1, testString1)
				HAM_TEST_EQUAL(string2.ToStlString(), testString2)
				STRING_EQUAL(string3, testString3.c_str())
			}

			// StringPart + String
			{
				StringPart string1(stlTestString1.c_str(), strlen(testString1));
				String string2(testString2);
				HAM_TEST_EQUAL(string1.ToStlString(), testString1)
				STRING_EQUAL(string2, testString2)

				String string3 = string1 + string2;

				HAM_TEST_EQUAL(string1.ToStlString(), testString1)
				STRING_EQUAL(string2, testString2)
				STRING_EQUAL(string3, testString3.c_str())
			}

			// StringPart + StringPart
			{
				StringPart string1(stlTestString1.c_str(), strlen(testString1));
				StringPart string2(stlTestString2.c_str(), strlen(testString2));
				HAM_TEST_EQUAL(string1.ToStlString(), testString1)
				HAM_TEST_EQUAL(string2.ToStlString(), testString2)

				String string3 = string1 + string2;

				HAM_TEST_EQUAL(string1.ToStlString(), testString1)
				HAM_TEST_EQUAL(string2.ToStlString(), testString2)
				STRING_EQUAL(string3, testString3.c_str())
			}

			std::swap(testString1, testString2);
		}
	}
}

void
StringTest::ToLowerUpper()
{
	// ToLower()
	{
		String string;
		string.ToLower();
		STRING_EQUAL(string, "");
	}

	{
		String string;
		String string2(string);
		string.ToLower();
		STRING_EQUAL(string, "");
		STRING_EQUAL(string2, "");
	}

	{
		String string("FooBar");
		string.ToLower();
		STRING_EQUAL(string, "foobar");
	}

	{
		String string("FooBar");
		String string2(string);
		string.ToLower();
		STRING_EQUAL(string, "foobar");
		STRING_EQUAL(string2, "FooBar");
	}

	// ToUpper()
	{
		String string;
		string.ToUpper();
		STRING_EQUAL(string, "");
	}

	{
		String string;
		String string2(string);
		string.ToUpper();
		STRING_EQUAL(string, "");
		STRING_EQUAL(string2, "");
	}

	{
		String string("FooBar");
		string.ToUpper();
		STRING_EQUAL(string, "FOOBAR");
	}

	{
		String string("FooBar");
		String string2(string);
		string.ToUpper();
		STRING_EQUAL(string, "FOOBAR");
		STRING_EQUAL(string2, "FooBar");
	}
}

void
StringTest::SubString()
{
	struct TestData {
		const char* string;
		size_t startOffset;
		size_t endOffset;
		const char* result;
	};

	const TestData testData[] = {
		{"", 0, 0, ""},
		{"foobar", 0, 0, ""},
		{"foobar", 3, 3, ""},
		{"foobar", 4, 2, ""},
		{"foobar", 6, 7, ""},
		{"foobar", 0, 6, "foobar"},
		{"foobar", 0, 8, "foobar"},
		{"foobar", 0, 4, "foob"},
		{"foobar", 2, 4, "ob"},
		{"foobar", 2, 6, "obar"},
		{"foobar", 2, 9, "obar"},
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		String string(testData[i].string);
		String subString =
			string.SubString(testData[i].startOffset, testData[i].endOffset);
		HAM_TEST_ADD_INFO(STRING_EQUAL(subString, testData[i].result),
						  "string: \"%s\", offsets: %zu - %zu",
						  testData[i].string,
						  testData[i].startOffset,
						  testData[i].endOffset)
	}
}

} // namespace ham::tests
