/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TESTS_STRING_TEST_H
#define HAM_TESTS_STRING_TEST_H


#include "test/TestFixture.h"


namespace ham {
namespace tests {


class StringTest : public test::TestFixture {
public:

			void				Constructor();
			void				Comparison();
			void				Assignment();
			void				Concatenation();

	// declare tests
	HAM_ADD_TEST_CASES(StringTest, 4,
		Constructor, Comparison, Assignment, Concatenation)

};


} // namespace tests
} // namespace ham


#endif // HAM_TESTS_STRING_TEST_H
