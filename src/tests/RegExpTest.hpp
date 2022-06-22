/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TESTS_REG_EXP_TEST_HPP
#define HAM_TESTS_REG_EXP_TEST_HPP

#include "test/TestFixture.hpp"

namespace ham::tests
{

class RegExpTest : public test::TestFixture
{
  public:
	void Constructor();
	void MatchRegularExpression();
	void MatchWildcard();

	// declare tests
	HAM_ADD_TEST_CASES(RegExpTest,
					   3,
					   Constructor,
					   MatchRegularExpression,
					   MatchWildcard)

  private:
	struct Matches;
};

} // namespace ham::tests

#endif // HAM_TESTS_REG_EXP_TEST_HPP
