/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/NotExpression.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


namespace ham {
namespace code {


NotExpression::NotExpression(Node* child)
	:
	fChild(child)
{
}


NotExpression::~NotExpression()
{
	delete fChild;
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
	return childList.IsEmpty() ? StringList::True() : StringList::False();
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


}	// namespace code
}	// namespace ham
