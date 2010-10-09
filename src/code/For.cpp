/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/For.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


using code::For;


For::For(Node* variable, Node* list, Node* block)
	:
	fVariable(variable),
	fList(list),
	fBlock(block)
{
}


For::~For()
{
	delete fVariable;
	delete fList;
	delete fBlock;
}


StringList
For::Evaluate(EvaluationContext& context)
{
	// TODO: Implement...
	return kFalseStringList;
}


code::Node*
For::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	if (Node* result = fVariable->Visit(visitor))
		return result;

	if (Node* result = fList->Visit(visitor))
		return result;

	return fBlock->Visit(visitor);
}


void
For::Dump(DumpContext& context) const
{
	context << "For(\n";
	context.BeginChildren();

	fVariable->Dump(context);
	fList->Dump(context);
	fBlock->Dump(context);

	context.EndChildren();
	context << ")\n";
}
