/*
 * Copyright 2010-2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/EvaluationContext.h"

#include <iostream>


namespace ham {
namespace code {


EvaluationContext::EvaluationContext(data::VariableDomain& globalVariables,
	data::TargetPool& targets)
	:
	fCompatibility(behavior::COMPATIBILITY_HAM),
	fBehavior(fCompatibility),
	fGlobalVariables(globalVariables),
	fRootScope(fGlobalVariables, NULL),
	fGlobalScope(&fRootScope),
	fLocalScope(NULL),
	fTargets(targets),
	fJumpCondition(JUMP_CONDITION_NONE),
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


}	// namespace code
}	// namespace ham
