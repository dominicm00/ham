/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "code/EvaluationContext.hpp"

#include <iostream>

namespace ham::code
{

EvaluationContext::EvaluationContext(data::VariableDomain& globalVariables,
									 data::TargetPool& targets)
	: fCompatibility(behavior::COMPATIBILITY_HAM),
	  fBehavior(fCompatibility),
	  fGlobalVariables(globalVariables),
	  fLocalScope(nullptr),
	  fBuiltInVariables(nullptr),
	  fTargets(targets),
	  fJumpCondition(JUMP_CONDITION_NONE),
	  fIncludeDepth(0),
	  fRuleCallDepth(0),
	  fOutput(&std::cout),
	  fErrorOutput(&std::cerr)
{
}

void
EvaluationContext::SetCompatibility(behavior::Compatibility compatibility)
{
	fCompatibility = compatibility;
	fBehavior = behavior::Behavior(fCompatibility);
}

} // namespace ham::code
