/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/RuleActions.h"


namespace ham {
namespace code {


RuleActions::RuleActions(const StringList& variables, const String& actions,
	uint32_t flags)
	:
	fVariables(variables),
	fActions(actions),
	fFlags(flags)
{
}


RuleActions::~RuleActions()
{
}


}	// namespace code
}	// namespace ham
