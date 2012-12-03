/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/Include.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


namespace ham {
namespace code {


Include::Include(Node* fileNames)
	:
	fFileNames(fileNames)
{
}


Include::~Include()
{
	delete fFileNames;
}


StringList
Include::Evaluate(EvaluationContext& context)
{
//		StringList fileNames = fFileNames->Evaluate(context);
// TODO: Include each file. Concatenate the results/return the last one?
	return StringList::False();
}


code::Node*
Include::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	return fFileNames->Visit(visitor);
}


void
Include::Dump(DumpContext& context) const
{
	context << "Include(\n";
	context.BeginChildren();

	fFileNames->Dump(context);

	context.EndChildren();
	context << ")\n";
}


}	// namespace code
}	// namespace ham
