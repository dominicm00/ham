/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/LocalVariableDeclaration.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


using code::LocalVariableDeclaration;


LocalVariableDeclaration::LocalVariableDeclaration(Node* variables,
	Node* initializer)
	:
	fVariables(variables),
	fInitializer(initializer)
{
}


LocalVariableDeclaration::~LocalVariableDeclaration()
{
	delete fVariables;
	delete fInitializer;
}


StringList
LocalVariableDeclaration::Evaluate(EvaluationContext& context)
{
	// get the variables
	const StringList& variables = fVariables->Evaluate(context);

	if (fInitializer != NULL) {
		// we have an initializer -- get the value and init the variables
		const StringList& value = fInitializer->Evaluate(context);

		for (StringList::const_iterator it = variables.begin();
				it != variables.end(); ++it) {
			context.LocalScope()->Set(*it, value);
		}

		return value;
	}

	// no initializer -- init with empty list
	for (StringList::const_iterator it = variables.begin();
			it != variables.end(); ++it) {
		context.LocalScope()->Set(*it, kFalseStringList);
	}

	return kFalseStringList;
}


code::Node*
LocalVariableDeclaration::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	if (Node* result = fVariables->Visit(visitor))
		return result;

	return fInitializer != NULL ? fInitializer->Visit(visitor) : NULL;
}


void
LocalVariableDeclaration::Dump(DumpContext& context) const
{
	context << "LocalVariableDeclaration(\n";
	context.BeginChildren();

	fVariables->Dump(context);

	if (fInitializer != NULL)
		fInitializer->Dump(context);

	context.EndChildren();
	context << ")\n";
}
