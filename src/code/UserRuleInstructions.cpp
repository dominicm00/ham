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
	const StringListList& parameters)
{
	// prepare the local variable scope
	data::VariableDomain localVariables;
	data::VariableScope localScope(localVariables, NULL);

	// set the number parameters ($(1) ... $(n))
	size_t parameterCount = parameters.size();
	for (size_t i = 0; i < parameterCount; i++) {
		if (!parameters[i].IsEmpty()) {
			char parameterName[16];
			snprintf(parameterName, sizeof(parameterName), "%zu", i);
			localVariables.Set(parameterName, parameters[i]);
		}
	}

	// set the named parameters
	StringList::Iterator it = fParameterNames.GetIterator();
	for (size_t i = 0; i < parameterCount && it.HasNext(); i++)
		localVariables.Set(it.Next(), parameters[i]);

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
