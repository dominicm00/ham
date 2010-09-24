/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/If.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


using code::If;


If::If(Node* expression, Node* block)
	:
	fExpression(expression),
	fBlock(block),
	fElseBlock(NULL)
{
}


StringList
If::Evaluate(EvaluationContext& context)
{
	return !fExpression->Evaluate(context).empty()
		? fBlock->Evaluate(context)
		: (fElseBlock != NULL
			? fBlock->Evaluate(context) : kFalseStringList);
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
