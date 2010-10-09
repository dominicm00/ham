/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/Assignment.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


using code::Assignment;


Assignment::Assignment(Node* left, AssignmentOperator operatorType, Node* right,
	Node* onTargets)
	:
	fLeft(left),
	fRight(right),
	fOnTargets(onTargets),
	fOperator(operatorType)
{
}


Assignment::~Assignment()
{
	delete fLeft;
	delete fRight;
	delete fOnTargets;
}


StringList
Assignment::Evaluate(EvaluationContext& context)
{
	// TODO: Implement!
	return kFalseStringList;
}


code::Node*
Assignment::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	if (Node* result = fLeft->Visit(visitor))
		return result;

	if (Node* result = fRight->Visit(visitor))
		return result;

	return fOnTargets != NULL ? fOnTargets->Visit(visitor) : NULL;
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
