/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/ActionsDefinition.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"
#include "code/Rule.h"
#include "data/RuleActions.h"


namespace ham {
namespace code {


ActionsDefinition::ActionsDefinition(uint32_t flags, const String& ruleName,
	Node* variables, const String& actions)
	:
	fRuleName(ruleName),
	fVariables(variables),
	fActions(actions),
	fFlags(flags)
{
	if (fVariables != NULL)
		fVariables->AcquireReference();
}


ActionsDefinition::~ActionsDefinition()
{
	if (fVariables != NULL)
		fVariables->ReleaseReference();
}


StringList
ActionsDefinition::Evaluate(EvaluationContext& context)
{
	// get the variables
	StringList variables;
	if (fVariables != NULL)
		variables = fVariables->Evaluate(context);

	// create and add the actions to the rule
	Rule& rule = context.Rules().LookupOrCreate(fRuleName);
	util::Reference<data::RuleActions> actions(
		new data::RuleActions(fRuleName, variables, fActions, fFlags), true);
	rule.SetActions(actions.Get());
	return StringList::False();
}


Node*
ActionsDefinition::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	return fVariables->Visit(visitor);
}


void
ActionsDefinition::Dump(DumpContext& context) const
{
	context << "ActionsDefinition(\"" << fRuleName << ", " << fFlags
		<< ",\n";
	context.BeginChildren();

	if (fVariables != NULL)
		fVariables->Dump(context);

	context << "actions:\n";
	context.BeginChildren();
	context << fActions;
	context.EndChildren();

	context.EndChildren();
	context << ")\n";
}


}	// namespace code
}	// namespace ham
