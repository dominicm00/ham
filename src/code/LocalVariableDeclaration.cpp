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
	// TODO: Implement!
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
