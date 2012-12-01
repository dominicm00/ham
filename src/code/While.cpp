/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/While.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


namespace ham {
namespace code {


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
	// perform the for loop
	StringList result;
	while (!fExpression->Evaluate(context).empty()) {
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


}	// namespace code
}	// namespace ham
