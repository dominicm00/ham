/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "code/If.hpp"

#include "code/DumpContext.hpp"
#include "code/EvaluationContext.hpp"

namespace ham::code
{

If::If(Node* expression, Node* block, Node* elseBlock)
	: fExpression(expression),
	  fBlock(block),
	  fElseBlock(elseBlock)
{
	fExpression->AcquireReference();
	fBlock->AcquireReference();
	if (fElseBlock != nullptr)
		fElseBlock->AcquireReference();
}

If::~If()
{
	fExpression->ReleaseReference();
	fBlock->ReleaseReference();
	if (fElseBlock != nullptr)
		fElseBlock->ReleaseReference();
}

StringList
If::Evaluate(EvaluationContext& context)
{
	return fExpression->Evaluate(context).IsTrue()
		? fBlock->Evaluate(context)
		: (fElseBlock != nullptr ? fElseBlock->Evaluate(context)
								 : StringList::False());
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

	return fElseBlock != nullptr ? fElseBlock->Visit(visitor) : nullptr;
}

void
If::Dump(DumpContext& context) const
{
	context << "If(\n";
	context.BeginChildren();

	fExpression->Dump(context);
	fBlock->Dump(context);
	if (fElseBlock != nullptr)
		fElseBlock->Dump(context);

	context.EndChildren();
	context << ")\n";
}

} // namespace ham::code
