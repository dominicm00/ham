/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/Assignment.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"
#include "data/TargetPool.h"


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
	const StringList& lhs = fLeft->Evaluate(context);
	const StringList& rhs = fRight->Evaluate(context);

	if (fOnTargets != NULL) {
		// Set the variables on the given targets.
		StringList targets = fOnTargets->Evaluate(context);
		for (StringList::const_iterator it = targets.begin();
				it != targets.end(); ++it) {
			// get the target and its variable domain
			data::Target* target = context.Targets().LookupOrCreate(*it);
			data::VariableDomain* domain = target->Variables(true);

			// set the variables
			for (StringList::const_iterator varIt = lhs.begin();
					varIt != lhs.end(); ++varIt) {
				switch (fOperator) {
					case ASSIGNMENT_OPERATOR_ASSIGN:
						domain->Set(*varIt, rhs);
						break;
					case ASSIGNMENT_OPERATOR_APPEND:
						std::copy(rhs.begin(), rhs.end(),
							domain->LookupOrCreate(*varIt).end());
						break;
					case ASSIGNMENT_OPERATOR_DEFAULT:
						if (domain->Lookup(*varIt) == NULL)
							domain->Set(*varIt, rhs);
						break;
				}
			}
		}
	} else {
		// No targets. If a local variable with the respective name exists, we
		// set it, otherwise we set one in a global scope.
		for (StringList::const_iterator it = lhs.begin();
				it != lhs.end(); ++it) {
			const String& variable = *it;

			// look for a local variable
			StringList* data = context.LocalScope()->Lookup(variable);
			bool created = false;
			if (data == NULL) {
				// no local variable -- check for a global one
				data = context.GlobalScope()->Lookup(variable);
				if (data == NULL) {
					// no existing global variable either -- create one
					data = &context.GlobalVariables()->LookupOrCreate(variable);
					created = true;
				}
			}

			switch (fOperator) {
				case ASSIGNMENT_OPERATOR_ASSIGN:
					*data = rhs;
					break;
				case ASSIGNMENT_OPERATOR_APPEND:
					std::copy(rhs.begin(), rhs.end(), data->end());
					break;
				case ASSIGNMENT_OPERATOR_DEFAULT:
					if (created)
						*data = rhs;
					break;
			}
		}
	}

	return rhs;
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
