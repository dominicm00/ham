/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "code/While.hpp"

#include "code/DumpContext.hpp"
#include "code/EvaluationContext.hpp"

namespace ham::code
{

While::While(Node* expression, Node* block)
	: fExpression(expression),
	  fBlock(block)
{
	fExpression->AcquireReference();
	fBlock->AcquireReference();
}

While::~While()
{
	fExpression->ReleaseReference();
	fBlock->ReleaseReference();
}

StringList
While::Evaluate(EvaluationContext& context)
{
	// perform the for loop
	StringList result;
	while (fExpression->Evaluate(context).IsTrue()) {
		// execute the block
		result = fBlock->Evaluate(context);

		// handle jump conditions
		switch (context.GetJumpCondition()) {
			case JUMP_CONDITION_NONE:
				break;
			case JUMP_CONDITION_BREAK:
				context.SetJumpCondition(JUMP_CONDITION_NONE);
				return result;
			case JUMP_CONDITION_CONTINUE:
				context.SetJumpCondition(JUMP_CONDITION_NONE);
				break;
			case JUMP_CONDITION_RETURN:
			case JUMP_CONDITION_JUMP_TO_EOF:
			case JUMP_CONDITION_EXIT:
				return result;
		}
	}

	return result;
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

} // namespace ham::code
