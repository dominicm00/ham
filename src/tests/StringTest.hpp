/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TESTS_STRING_TEST_HPP
#define HAM_TESTS_STRING_TEST_HPP

#include "test/TestFixture.hpp"

namespace ham::tests
{

class StringTest : public test::TestFixture
{
  public:
	void Constructor();
	void CastOperator();
	void Comparison();
	void Assignment();
	void Concatenation();
	void ToLowerUpper();
	void SubString();

	// declare tests
	HAM_ADD_TEST_CASES(
		StringTest,
		7,
		Constructor,
		CastOperator,
		Comparison,
		Assignment,
		Concatenation,
		ToLowerUpper,
		SubString
	)
};

} // namespace ham::tests

#endif // HAM_TESTS_STRING_TEST_HPP
