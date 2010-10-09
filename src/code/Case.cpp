/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/Case.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


using code::Case;


Case::Case(const String& pattern, Node* block)
	:
	fPattern(pattern),
	fBlock(block)
{
}


Case::~Case()
{
	delete fBlock;
}


StringList
Case::Evaluate(EvaluationContext& context)
{
	return fBlock->Evaluate(context);
}


code::Node*
Case::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	return fBlock->Visit(visitor);
}


void
Case::Dump(DumpContext& context) const
{
	context << "Case(\"" << fPattern << "\",\n";
	context.BeginChildren();

	fBlock->Dump(context);

	context.EndChildren();
	context << ")\n";
}
