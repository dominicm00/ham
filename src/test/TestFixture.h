/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TEST_TEST_FIXTURE_H
#define HAM_TEST_TEST_FIXTURE_H


#include <vector>

#include "data/StringList.h"
#include "test/TestException.h"


namespace ham {
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
									const char* element10 = NULL,
									const char* element11 = NULL,
									const char* element12 = NULL,
									const char* element13 = NULL,
									const char* element14 = NULL,
									const char* element15 = NULL,
									const char* element16 = NULL,
									const char* element17 = NULL,
									const char* element18 = NULL,
									const char* element19 = NULL,
									const char* element20 = NULL);
	static	data::StringList	MakeStringList(
									const std::vector<std::string>& testList);
	static	data::StringListList MakeStringListList(
									const std::vector<std::vector<
										std::string> >& testListList);

	template<typename Type>
	static	std::string			ValueToString(const Type& value);
};


template<typename Type>
struct GetNonReferenceType {
	typedef Type type;
};


template<typename Type>
struct GetNonReferenceType<Type&> {
	typedef Type type;
};


template<typename Type>
struct GetNonReferenceType<const Type&> {
	typedef Type type;
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
		HAM_CALL_MACRO1(testCount, HAM_ADD_TEST_CASES_VISIT,		\
			fixture, __VA_ARGS__)									\
	}

#define HAM_ADD_TEST_CASES_VISIT(fixture, testCase)	\
	visitor.Visit(#testCase, &fixture::testCase);


#define HAM_CALL_MACRO1(count, macro, fixArgument,...)		\
	HAM_CALL_MACRO1_##count(macro, fixArgument, __VA_ARGS__)

#define HAM_CALL_MACRO1_0(macro, fixArgument)
#define HAM_CALL_MACRO1_1(macro, fixArgument, argument)			\
	macro(fixArgument, argument)
#define HAM_CALL_MACRO1_2(macro, fixArgument, argument, ...)	\
	HAM_CALL_MACRO1_1(macro, fixArgument, argument)				\
	HAM_CALL_MACRO1_1(macro, fixArgument, __VA_ARGS__)
#define HAM_CALL_MACRO1_3(macro, fixArgument, argument, ...)	\
	HAM_CALL_MACRO1_1(macro, fixArgument, argument)				\
	HAM_CALL_MACRO1_2(macro, fixArgument, __VA_ARGS__)
#define HAM_CALL_MACRO1_4(macro, fixArgument, argument, ...)	\
	HAM_CALL_MACRO1_1(macro, fixArgument, argument)				\
	HAM_CALL_MACRO1_3(macro, fixArgument, __VA_ARGS__)
#define HAM_CALL_MACRO1_5(macro, fixArgument, argument, ...)	\
	HAM_CALL_MACRO1_1(macro, fixArgument, argument)				\
	HAM_CALL_MACRO1_4(macro, fixArgument, __VA_ARGS__)
#define HAM_CALL_MACRO1_6(macro, fixArgument, argument, ...)	\
	HAM_CALL_MACRO1_1(macro, fixArgument, argument)				\
	HAM_CALL_MACRO1_5(macro, fixArgument, __VA_ARGS__)
#define HAM_CALL_MACRO1_7(macro, fixArgument, argument, ...)	\
	HAM_CALL_MACRO1_1(macro, fixArgument, argument)				\
	HAM_CALL_MACRO1_6(macro, fixArgument, __VA_ARGS__)
#define HAM_CALL_MACRO1_8(macro, fixArgument, argument, ...)	\
	HAM_CALL_MACRO1_1(macro, fixArgument, argument)				\
	HAM_CALL_MACRO1_7(macro, fixArgument, __VA_ARGS__)
#define HAM_CALL_MACRO1_9(macro, fixArgument, argument, ...)	\
	HAM_CALL_MACRO1_1(macro, fixArgument, argument)				\
	HAM_CALL_MACRO1_8(macro, fixArgument, __VA_ARGS__)

#define HAM_CALL_MACRO1_10(macro, fixArgument, argument, ...)	\
	HAM_CALL_MACRO1_1(macro, fixArgument, argument)				\
	HAM_CALL_MACRO1_9(macro, fixArgument, __VA_ARGS__)
#define HAM_CALL_MACRO1_11(macro, fixArgument, argument, ...)	\
	HAM_CALL_MACRO1_1(macro, fixArgument, argument)				\
	HAM_CALL_MACRO1_10(macro, fixArgument, __VA_ARGS__)
#define HAM_CALL_MACRO1_12(macro, fixArgument, argument, ...)	\
	HAM_CALL_MACRO1_1(macro, fixArgument, argument)				\
	HAM_CALL_MACRO1_11(macro, fixArgument, __VA_ARGS__)
#define HAM_CALL_MACRO1_13(macro, fixArgument, argument, ...)	\
	HAM_CALL_MACRO1_1(macro, fixArgument, argument)				\
	HAM_CALL_MACRO1_12(macro, fixArgument, __VA_ARGS__)
#define HAM_CALL_MACRO1_14(macro, fixArgument, argument, ...)	\
	HAM_CALL_MACRO1_1(macro, fixArgument, argument)				\
	HAM_CALL_MACRO1_13(macro, fixArgument, __VA_ARGS__)
#define HAM_CALL_MACRO1_15(macro, fixArgument, argument, ...)	\
	HAM_CALL_MACRO1_1(macro, fixArgument, argument)				\
	HAM_CALL_MACRO1_14(macro, fixArgument, __VA_ARGS__)
#define HAM_CALL_MACRO1_16(macro, fixArgument, argument, ...)	\
	HAM_CALL_MACRO1_1(macro, fixArgument, argument)				\
	HAM_CALL_MACRO1_15(macro, fixArgument, __VA_ARGS__)
#define HAM_CALL_MACRO1_17(macro, fixArgument, argument, ...)	\
	HAM_CALL_MACRO1_1(macro, fixArgument, argument)				\
	HAM_CALL_MACRO1_16(macro, fixArgument, __VA_ARGS__)
#define HAM_CALL_MACRO1_18(macro, fixArgument, argument, ...)	\
	HAM_CALL_MACRO1_1(macro, fixArgument, argument)				\
	HAM_CALL_MACRO1_17(macro, fixArgument, __VA_ARGS__)
#define HAM_CALL_MACRO1_19(macro, fixArgument, argument, ...)	\
	HAM_CALL_MACRO1_1(macro, fixArgument, argument)				\
	HAM_CALL_MACRO1_18(macro, fixArgument, __VA_ARGS__)


#define HAM_TEST_THROW(...)													\
	{																		\
		throw test::TestException(__FILE__, __LINE__, __VA_ARGS__);			\
	}

#define HAM_TEST_VERIFY(condition)											\
	if (!(condition))														\
		HAM_TEST_THROW("Test condition doesn't hold: %s", #condition);


#define HAM_TEST_EQUAL(actual, expected)									\
	{																		\
		typedef typename													\
			ham::test::GetNonReferenceType<decltype((actual))>::type		\
				_TestActualType;											\
		typedef typename													\
			ham::test::GetNonReferenceType<decltype((expected))>::type		\
				_TestExpectedType;											\
		const _TestActualType& _testActual = actual;						\
		const _TestExpectedType& _testExpected = expected;					\
		if (_testActual != _testExpected) {									\
			throw test::TestException(__FILE__, __LINE__,					\
				"Test comparison failed:\n  expected: \"%s\"\n"				\
				"  actual:   \"%s\" (\"%s\")",								\
				test::TestFixture::ValueToString(_testExpected).c_str(),	\
				test::TestFixture::ValueToString(_testActual).c_str(),		\
				#actual);													\
		}																	\
	}


#define HAM_TEST_ADD_INFO(statement, message, ...)							\
	try {																	\
		statement;															\
	} catch (test::TestException& exception) {								\
		exception.ThrowWithExtendedMessage(message, __VA_ARGS__);			\
	}


} // namespace test
} // namespace ham


#endif // HAM_TEST_TEST_FIXTURE_H
