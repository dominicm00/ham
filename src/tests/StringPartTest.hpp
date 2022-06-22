/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TESTS_STRING_PART_TEST_HPP
#define HAM_TESTS_STRING_PART_TEST_HPP

#include "test/TestFixture.hpp"

namespace ham::tests
{

class StringPartTest : public test::TestFixture
{
  public:
	void Constructor();
	void SetTo();
	void Comparison();
	void Assignment();

	// declare tests
	HAM_ADD_TEST_CASES(StringPartTest,
					   4,
					   Constructor,
					   SetTo,
					   Comparison,
					   Assignment)
};

} // namespace ham::tests

#endif // HAM_TESTS_STRING_PART_TEST_HPP
