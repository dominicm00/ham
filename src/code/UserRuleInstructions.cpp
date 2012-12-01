/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/UserRuleInstructions.h"

#include <stdio.h>

#include "code/EvaluationContext.h"
#include "code/Node.h"
#include "data/VariableScope.h"


namespace ham {
namespace code {


class Node;


UserRuleInstructions::UserRuleInstructions(const StringList& parameterNames,
	Node* block)
	:
	fParameterNames(parameterNames),
	fBlock(block)
{
}


StringList
UserRuleInstructions::Evaluate(EvaluationContext& context,
	const StringList* parameters, size_t parameterCount)
{
	// prepare the local variable scope
	data::VariableDomain localVariables;
	data::VariableScope localScope(localVariables, NULL);

	// set the number parameters ($(1) ... $(n))
	for (size_t i = 0; i < parameterCount; i++) {
		if (!parameters[i].empty()) {
			char parameterName[16];
			snprintf(parameterName, sizeof(parameterName), "%zu", i);
			localVariables.Set(parameterName, parameters[i]);
		}
	}

	// set the named parameters
	StringList::const_iterator it = fParameterNames.begin();
	for (size_t i = 0; i < parameterCount && it != fParameterNames.end();
			i++, ++it) {
		localVariables.Set(*it, parameters[i]);
	}

	// set the local variable scope
	data::VariableScope* oldLocalScope = context.LocalScope();
	context.SetLocalScope(&localScope);

	// execute the rule block
	StringList result = fBlock->Evaluate(context);

	// reinstate the old local variable scope
	context.SetLocalScope(oldLocalScope);

	return result;
}


}	// namespace code
}	// namespace ham
