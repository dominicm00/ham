/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TESTS_VARIABLE_EXPANSION_TEST_HPP
#define HAM_TESTS_VARIABLE_EXPANSION_TEST_HPP

#include "code/EvaluationContext.hpp"
#include "data/TargetPool.hpp"
#include "data/VariableDomain.hpp"
#include "test/TestFixture.hpp"

namespace ham::tests
{

class VariableExpansionTest : public test::TestFixture
{
  public:
	VariableExpansionTest();

	void InitTestCase();
	void CleanupTestCase();

	void None();
	void PathSelectorsEmpty();

	// declare tests
	HAM_ADD_TEST_CASES(VariableExpansionTest, 2, None, PathSelectorsEmpty)

  private:
	StringList _Evaluate(const String& string);

  private:
	data::VariableDomain* fGlobalVariables;
	data::TargetPool* fTargets;
	code::EvaluationContext* fEvaluationContext;
};

} // namespace ham::tests

#endif // HAM_TESTS_VARIABLE_EXPANSION_TEST_HPP
