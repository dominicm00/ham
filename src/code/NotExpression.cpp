/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/NotExpression.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


using code::NotExpression;


NotExpression::NotExpression(Node* child)
	:
	fChild(child)
{
}


StringList
NotExpression::Evaluate(EvaluationContext& context)
{
	StringList childList = fChild->Evaluate(context);
	return childList.empty() ? kTrueStringList : kFalseStringList;
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
