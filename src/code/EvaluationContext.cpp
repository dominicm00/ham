/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/EvaluationContext.h"


namespace code {


EvaluationContext::EvaluationContext(data::VariableDomain& globalVariables,
	data::TargetPool& targets)
	:
	fGlobalVariables(globalVariables),
	fRootScope(fGlobalVariables, NULL),
	fGlobalScope(&fRootScope),
	fLocalScope(NULL),
	fTargets(targets),
	fJumpCondition(JUMP_CONDITION_NONE)
{
}


}	// namespace code
