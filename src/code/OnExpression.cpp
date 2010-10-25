/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/OnExpression.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


using code::OnExpression;


OnExpression::OnExpression(Node* object, Node* expression)
	:
	fObject(object),
	fExpression(expression)
{
}


OnExpression::~OnExpression()
{
	delete fObject;
	delete fExpression;
}


StringList
OnExpression::Evaluate(EvaluationContext& context)
{
//		StringList object = fObject->Evaluate(context);
	// TODO: Push object context!

	StringList result = fExpression->Evaluate(context);

	return result;
}


code::Node*
OnExpression::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	if (Node* result = fObject->Visit(visitor))
		return result;

	return fExpression->Visit(visitor);
}


void
OnExpression::Dump(DumpContext& context) const
{
	context << "OnExpression(\n";
	context.BeginChildren();

	fObject->Dump(context);
	fExpression->Dump(context);

	context.EndChildren();
	context << ")\n";
}
