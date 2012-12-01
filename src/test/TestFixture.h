/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TEST_TEST_FIXTURE_H
#define HAM_TEST_TEST_FIXTURE_H


#include "data/String.h"
#include "test/TestException.h"


namespace test {


class TestFixture {
public:
	static	data::StringList	MakeStringList(const char* element1,
									const char* element2 = NULL,
									const char* element3 = NULL,
									const char* element4 = NULL,
									const char* element5 = NULL,
									const char* element6 = NULL,
									const char* element7 = NULL,
									const char* element8 = NULL,
									const char* element9 = NULL,
									const char* element10 = NULL);

	template<typename Type>
	static	std::string			ValueToString(const Type& value);
};


#define HAM_ADD_TEST_CASES(fixture, testCount,...)					\
public:																\
	static const char* TestFixtureName()							\
	{																\
		return #fixture;											\
	}																\
																	\
	template<typename Visitor>										\
	static void VisitTestCases(Visitor& visitor)					\
	{																\
		HAM_ADD_TEST_CASES_VISIT##testCount(fixture, __VA_ARGS__);	\
	}

#define HAM_ADD_TEST_CASES_VISIT1(fixture, testCase)		\
	visitor.Visit(#testCase, &fixture::testCase);
#define HAM_ADD_TEST_CASES_VISIT2(fixture, testCase,...)	\
	HAM_ADD_TEST_CASES_VISIT1(fixture, testCase)			\
	HAM_ADD_TEST_CASES_VISIT1(fixture, __VA_ARGS__)
#define HAM_ADD_TEST_CASES_VISIT3(fixture, testCase,...)	\
	HAM_ADD_TEST_CASES_VISIT1(fixture, testCase)			\
	HAM_ADD_TEST_CASES_VISIT2(fixture, __VA_ARGS__)
#define HAM_ADD_TEST_CASES_VISIT4(fixture, testCase,...)	\
	HAM_ADD_TEST_CASES_VISIT1(fixture, testCase)			\
	HAM_ADD_TEST_CASES_VISIT3(fixture, __VA_ARGS__)
#define HAM_ADD_TEST_CASES_VISIT5(fixture, testCase,...)	\
	HAM_ADD_TEST_CASES_VISIT1(fixture, testCase)			\
	HAM_ADD_TEST_CASES_VISIT4(fixture, __VA_ARGS__)
#define HAM_ADD_TEST_CASES_VISIT6(fixture, testCase,...)	\
	HAM_ADD_TEST_CASES_VISIT1(fixture, testCase)			\
	HAM_ADD_TEST_CASES_VISIT5(fixture, __VA_ARGS__)
#define HAM_ADD_TEST_CASES_VISIT7(fixture, testCase,...)	\
	HAM_ADD_TEST_CASES_VISIT1(fixture, testCase)			\
	HAM_ADD_TEST_CASES_VISIT6(fixture, __VA_ARGS__)
#define HAM_ADD_TEST_CASES_VISIT8(fixture, testCase,...)	\
	HAM_ADD_TEST_CASES_VISIT1(fixture, testCase)			\
	HAM_ADD_TEST_CASES_VISIT7(fixture, __VA_ARGS__)
#define HAM_ADD_TEST_CASES_VISIT9(fixture, testCase,...)	\
	HAM_ADD_TEST_CASES_VISIT1(fixture, testCase)			\
	HAM_ADD_TEST_CASES_VISIT8(fixture, __VA_ARGS__)
#define HAM_ADD_TEST_CASES_VISIT10(fixture, testCase,...)	\
	HAM_ADD_TEST_CASES_VISIT1(fixture, testCase)			\
	HAM_ADD_TEST_CASES_VISIT9(fixture, __VA_ARGS__)


#define HAM_TEST_VERIFY(condition)								\
	if (!(condition)) {											\
		throw test::TestException(__FILE__, __LINE__,			\
			"Test condition doesn't hold: %s", #condition);		\
	}

#define HAM_TEST_EQUAL(actual, expected)									\
	{																		\
		const typeof(actual)& _testActual = actual;							\
		const typeof(expected)& _testExpected = expected;					\
		if (_testActual != _testExpected) {									\
			throw test::TestException(__FILE__, __LINE__,					\
				"Test comparison failed: expected: \"%s\", "				\
				"actual: \"%s\" (\"%s\")",									\
				test::TestFixture::ValueToString(_testExpected).c_str(),	\
				test::TestFixture::ValueToString(_testActual).c_str(),		\
				#actual);													\
		}																	\
	}


} // namespace test


#endif // HAM_TEST_TEST_FIXTURE_H
