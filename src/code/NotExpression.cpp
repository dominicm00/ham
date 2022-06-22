/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "code/NotExpression.hpp"

#include "code/DumpContext.hpp"
#include "code/EvaluationContext.hpp"

namespace ham
{
namespace code
{

NotExpression::NotExpression(Node* child)
	: fChild(child)
{
	fChild->AcquireReference();
}

NotExpression::~NotExpression()
{
	fChild->ReleaseReference();
}

code::Node*
NotExpression::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	return fChild->Visit(visitor);
}

StringList
NotExpression::Evaluate(EvaluationContext& context)
{
	StringList childList = fChild->Evaluate(context);
	return childList.IsTrue() ? StringList::False() : StringList::True();
}

void
NotExpression::Dump(DumpContext& context) const
{
	context << "NotExpression(\n";
	context.BeginChildren();

	fChild->Dump(context);

	context.EndChildren();
	context << ")\n";
}

} // namespace code
} // namespace ham
