/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "code/List.hpp"

#include "code/DumpContext.hpp"
#include "code/EvaluationContext.hpp"

namespace ham::code
{

List::List() {}

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

	return nullptr;
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

} // namespace ham::code
