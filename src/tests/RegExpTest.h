/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TESTS_REG_EXP_TEST_H
#define HAM_TESTS_REG_EXP_TEST_H

#include "test/TestFixture.h"

namespace ham
{
namespace tests
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

} // namespace tests
} // namespace ham

#endif // HAM_TESTS_REG_EXP_TEST_H
