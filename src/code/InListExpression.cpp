/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "code/InListExpression.hpp"

#include "code/DumpContext.hpp"
#include "code/EvaluationContext.hpp"

#include <algorithm>

namespace ham::code
{

InListExpression::InListExpression(Node* left, Node* right)
	: fLeft(left),
	  fRight(right)
{
	fLeft->AcquireReference();
	fRight->AcquireReference();
}

InListExpression::~InListExpression()
{
	fLeft->ReleaseReference();
	fRight->ReleaseReference();
}

StringList
InListExpression::Evaluate(EvaluationContext& context)
{
	StringList left = fLeft->Evaluate(context);
	StringList right = fRight->Evaluate(context);

	for (StringList::Iterator it = left.GetIterator(); it.HasNext();) {
		if (!right.Contains(it.Next()))
			return StringList::False();
	}

	return StringList::True();
}

code::Node*
InListExpression::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	if (Node* result = fLeft->Visit(visitor))
		return result;

	return fRight->Visit(visitor);
}

void
InListExpression::Dump(DumpContext& context) const
{
	context << "InListExpression(\n";
	context.BeginChildren();

	fLeft->Dump(context);
	fRight->Dump(context);

	context.EndChildren();
	context << ")\n";
}

} // namespace ham::code
