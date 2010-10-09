/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/ActionsDefinition.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


using code::ActionsDefinition;


ActionsDefinition::ActionsDefinition(uint32_t flags, const String& identifier,
	Node* variables, const String& actions)
	:
	fIdentifier(identifier),
	fVariables(variables),
	fActions(actions),
	fFlags(flags)
{
}


ActionsDefinition::~ActionsDefinition()
{
	delete fVariables;
}


StringList
ActionsDefinition::Evaluate(EvaluationContext& context)
{
	// TODO: Implement!
	return kFalseStringList;
}


code::Node*
ActionsDefinition::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	return fVariables->Visit(visitor);
}


void
ActionsDefinition::Dump(DumpContext& context) const
{
	context << "ActionsDefinition(\"" << fIdentifier << ", " << fFlags
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
