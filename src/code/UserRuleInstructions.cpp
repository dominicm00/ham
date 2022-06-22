/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "code/UserRuleInstructions.hpp"

#include <stdio.h>

#include "code/EvaluationContext.hpp"
#include "code/Node.hpp"
#include "data/VariableScope.hpp"

namespace ham::code
{

class Node;

UserRuleInstructions::UserRuleInstructions(const StringList& parameterNames,
										   Node* block)
	: fParameterNames(parameterNames),
	  fBlock(block)
{
	fBlock->AcquireReference();
}

UserRuleInstructions::~UserRuleInstructions()
{
	fBlock->ReleaseReference();
}

StringList
UserRuleInstructions::Evaluate(EvaluationContext& context,
							   const StringListList& parameters)
{
	// create a variable domain for the built-in variables (the numbered ones
	// and "<" and ">")
	data::VariableDomain builtInVariables;

	// set the number parameters ($(1) ... $(n))
	size_t parameterCount = parameters.size();
	for (size_t i = 0; i < parameterCount; i++) {
		if (!parameters[i].IsEmpty()) {
			char parameterName[24];
			snprintf(parameterName, sizeof(parameterName), "%zu", i + 1);
			builtInVariables.Set(parameterName, parameters[i]);
		}
	}

	// set the '<' and '>' variables
	if (parameterCount >= 1 && !parameters[0].IsEmpty())
		builtInVariables.Set("<", parameters[0]);

	if (parameterCount >= 2 && !parameters[1].IsEmpty())
		builtInVariables.Set(">", parameters[1]);

	// prepare the local variable scope (for the named parameters)
	data::VariableScope* oldLocalScope = context.LocalScope();
	data::VariableDomain localVariables;
	data::VariableScope localScope(localVariables, oldLocalScope);
	// TODO: This is jam compatible behavior. It would be more logical to
	// have a null parent for the new scope, so the previous local variables
	// cannot be seen in the rule block.

	// set the named parameters
	StringList::Iterator it = fParameterNames.GetIterator();
	for (size_t i = 0; i < parameterCount && it.HasNext(); i++)
		localVariables.Set(it.Next(), parameters[i]);

	// set the local variable scope and the built-in variables
	context.SetLocalScope(&localScope);

	data::VariableDomain* oldBuiltInVariables = context.BuiltInVariables();
	context.SetBuiltInVariables(&builtInVariables);

	// execute the rule block
	StringList result = fBlock->Evaluate(context);

	if (context.GetJumpCondition() == JUMP_CONDITION_RETURN)
		context.SetJumpCondition(JUMP_CONDITION_NONE);

	// reinstate the old local variable scope and the built-in variables
	context.SetLocalScope(oldLocalScope);
	context.SetBuiltInVariables(oldBuiltInVariables);

	return result;
}

} // namespace ham::code
