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
		delete fChildren[i];
}


StringList
List::Evaluate(EvaluationContext& context)
{
	StringList result;
	size_t childCount = fChildren.size();
	for (size_t i = 0; i < childCount; i++) {
		StringList childList = fChildren[i]->Evaluate(context);
		result.insert(result.end(), childList.begin(), childList.end());
	}

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
