/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TESTS_STRING_LIST_TEST_H
#define HAM_TESTS_STRING_LIST_TEST_H


#include "test/TestFixture.h"


namespace ham {
namespace tests {


class StringListTest : public test::TestFixture {
public:

			void				Constructor();
			void				ElementAccess();
			void				SubList();
			void				Contains();
			void				Constants();
			void				Comparison();
			void				Assignment();
			void				Concatenation();
			void				Clear();
			void				Join();
			void				Multiply();
			void				Iteration();

	// declare tests
	HAM_ADD_TEST_CASES(StringListTest, 12,
		Constructor, ElementAccess, SubList, Contains, Constants, Comparison,
		Assignment, Concatenation, Clear, Join, Multiply, Iteration)
};


} // namespace tests
} // namespace ham


#endif // HAM_TESTS_STRING_LIST_TEST_H
