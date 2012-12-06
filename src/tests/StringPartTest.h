/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TESTS_STRING_PART_TEST_H
#define HAM_TESTS_STRING_PART_TEST_H


#include "test/TestFixture.h"


namespace ham {
namespace tests {


class StringPartTest : public test::TestFixture {
public:

			void				Constructor();
			void				Comparison();
			void				Assignment();

	// declare tests
	HAM_ADD_TEST_CASES(StringPartTest, 3,
		Constructor, Comparison, Assignment)

};


} // namespace tests
} // namespace ham


#endif // HAM_TESTS_STRING_PART_TEST_H
