/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/If.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


using code::If;


If::If(Node* expression, Node* block, Node* elseBlock)
	:
	fExpression(expression),
	fBlock(block),
	fElseBlock(elseBlock)
{
}


If::~If()
{
	delete fExpression;
	delete fBlock;
	delete fElseBlock;
}


StringList
If::Evaluate(EvaluationContext& context)
{
	return !fExpression->Evaluate(context).empty()
		? fBlock->Evaluate(context)
		: (fElseBlock != NULL
			? fBlock->Evaluate(context) : data::kFalseStringList);
}


code::Node*
If::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	if (Node* result = fExpression->Visit(visitor))
		return result;

	if (Node* result = fBlock->Visit(visitor))
		return result;

	return fElseBlock != NULL ? fElseBlock->Visit(visitor) : NULL;
}


void
If::Dump(DumpContext& context) const
{
	context << "If(\n";
	context.BeginChildren();

	fExpression->Dump(context);
	fBlock->Dump(context);
	if (fElseBlock != NULL)
		fElseBlock->Dump(context);

	context.EndChildren();
	context << ")\n";
}
