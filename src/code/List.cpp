/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/List.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


namespace ham {
namespace code {


List::List()
{
}


List::~List()
{
	size_t childCount = fChildren.size();
	for (size_t i = 0; i < childCount; i++)
		fChildren[i]->ReleaseReference();
}


StringList
List::Evaluate(EvaluationContext& context)
{
	StringList result;
	size_t childCount = fChildren.size();
	for (size_t i = 0; i < childCount; i++)
		result.Append(fChildren[i]->Evaluate(context));

	return result;
}


code::Node*
List::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	size_t childCount = fChildren.size();
	for (size_t i = 0; i < childCount; i++) {
		if (Node* result = fChildren[i]->Visit(visitor))
			return result;
	}

	return NULL;
}


void
List::Dump(DumpContext& context) const
{
	context << "List(\n";
	context.BeginChildren();

	size_t childCount = fChildren.size();
	for (size_t i = 0; i < childCount; i++)
		fChildren[i]->Dump(context);

	context.EndChildren();
	context << ")\n";
}


}	// namespace code
}	// namespace ham
