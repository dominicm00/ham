/*
 * Copyright 2022, Dominic Martinez, dom@dominicm.dev.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TESTS_RULESET_TEST_H
#define HAM_TESTS_RULESET_TEST_H

#include "test/TestFixture.h"

namespace ham
{
namespace tests
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

} // namespace tests
} // namespace ham

#endif // HAM_TESTS_RULESET_TEST_H
