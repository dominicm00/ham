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
			void				Contains();
			void				Constants();
			void				Comparison();
			void				Assignment();
			void				Concatenation();
			void				Clear();
			void				Iteration();

	// declare tests
	HAM_ADD_TEST_CASES(StringListTest, 9,
		Constructor, ElementAccess, Contains, Constants, Comparison, Assignment,
		Concatenation, Clear, Iteration)
};


} // namespace tests
} // namespace ham


#endif // HAM_TESTS_STRING_LIST_TEST_H
