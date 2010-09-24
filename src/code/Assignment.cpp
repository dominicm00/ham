/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/Assignment.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


using code::Assignment;


Assignment::Assignment(Node* left, AssignmentOperator operatorType, List* right,
	List* onTargets)
	:
	fLeft(left),
	fRight(right),
	fOnTargets(onTargets),
	fOperator(operatorType)
{
}


StringList
Assignment::Evaluate(EvaluationContext& context)
{
	// TODO: Implement!
	return kFalseStringList;
}


void
Assignment::Dump(DumpContext& context) const
{
	context << "Assignment[";
	switch (fOperator) {
		case ASSIGNMENT_OPERATOR_ASSIGN:
			context << "=";
			break;
		case ASSIGNMENT_OPERATOR_APPEND:
			context << "+=";
			break;
		case ASSIGNMENT_OPERATOR_DEFAULT:
			context << "?=";
			break;
	}
	context << "](\n";
	context.BeginChildren();

	fLeft->Dump(context);
	fRight->Dump(context);

	if (fOnTargets != NULL)
		fOnTargets->Dump(context);

	context.EndChildren();
	context << ")\n";
}
