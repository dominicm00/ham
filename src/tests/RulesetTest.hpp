/*
 * Copyright 2022, Dominic Martinez, dom@dominicm.dev.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TESTS_RULESET_TEST_HPP
#define HAM_TESTS_RULESET_TEST_HPP

#include "test/TestFixture.hpp"

namespace ham::tests
{

/**
 * Ruleset tests to make sure there's no weird escaping nonsense going on with
 * the script.
 */
class RulesetTest : public test::TestFixture
{
  public:
	void JamRuleset();
	void HamRuleset();

	// declare tests
	HAM_ADD_TEST_CASES(RulesetTest, 2, JamRuleset, HamRuleset)
};

} // namespace ham::tests

#endif // HAM_TESTS_RULESET_TEST_HPP
