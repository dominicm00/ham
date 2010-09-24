/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/InListExpression.h"

#include <algorithm>

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


using code::InListExpression;


InListExpression::InListExpression(Node* left, Node* right)
	:
	fLeft(left),
	fRight(right)
{
}


StringList
InListExpression::Evaluate(EvaluationContext& context)
{
	StringList left = fLeft->Evaluate(context);
	StringList right = fRight->Evaluate(context);

	for (StringList::iterator it = left.begin(); it != left.end(); ++it) {
		if (std::find(right.begin(), right.end(), *it) == right.end())
			return kFalseStringList;
	}

	return kTrueStringList;
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
