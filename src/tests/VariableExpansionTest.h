/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TESTS_VARIABLE_EXPANSION_TEST_H
#define HAM_TESTS_VARIABLE_EXPANSION_TEST_H


#include "code/EvaluationContext.h"
#include "data/TargetPool.h"
#include "data/VariableDomain.h"
#include "test/TestFixture.h"


namespace ham {
namespace tests {


class VariableExpansionTest : public test::TestFixture {
public:
								VariableExpansionTest();

			void				InitTestCase();
			void				CleanupTestCase();

			void				None();
			void				Simple();
			void				Subscripts();
			void				Operations();
			void				PathOperationsEmpty();
			void				PathOperationsNoGristNoArchive();

	// declare tests
	HAM_ADD_TEST_CASES(VariableExpansionTest, 6,
		None, Simple, Subscripts, Operations, PathOperationsEmpty,
		PathOperationsNoGristNoArchive)

private:
			StringList			_Evaluate(const String& string);

private:
			data::VariableDomain* fGlobalVariables;
			data::TargetPool*	fTargets;
			code::EvaluationContext* fEvaluationContext;
};


} // namespace tests
} // namespace ham


#endif // HAM_TESTS_VARIABLE_EXPANSION_TEST_H
