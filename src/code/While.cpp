/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/While.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


using code::While;


While::While(Node* expression, Node* block)
	:
	fExpression(expression),
	fBlock(block)
{
}


While::~While()
{
	delete fExpression;
	delete fBlock;
}


StringList
While::Evaluate(EvaluationContext& context)
{
	// TODO: Implement...
	return kFalseStringList;
}


code::Node*
While::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	if (Node* result = fExpression->Visit(visitor))
		return result;

	return fBlock->Visit(visitor);
}


void
While::Dump(DumpContext& context) const
{
	context << "While(\n";
	context.BeginChildren();

	fExpression->Dump(context);
	fBlock->Dump(context);

	context.EndChildren();
	context << ")\n";
}
