/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/Assignment.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"
#include "data/TargetPool.h"


namespace ham {
namespace code {


Assignment::Assignment(Node* left, AssignmentOperator operatorType, Node* right,
	Node* onTargets)
	:
	fLeft(left),
	fRight(right),
	fOnTargets(onTargets),
	fOperator(operatorType)
{
	fLeft->AcquireReference();
	fRight->AcquireReference();
	if (fOnTargets != NULL)
		fOnTargets->AcquireReference();
}


Assignment::~Assignment()
{
	fLeft->ReleaseReference();
	fRight->ReleaseReference();
	if (fOnTargets != NULL)
		fOnTargets->ReleaseReference();
}


StringList
Assignment::Evaluate(EvaluationContext& context)
{
	const StringList& lhs = fLeft->Evaluate(context);
	const StringList& rhs = fRight->Evaluate(context);

	if (fOnTargets != NULL) {
		// Set the variables on the given targets.
		StringList targets = fOnTargets->Evaluate(context);
		for (StringList::Iterator it = targets.GetIterator(); it.HasNext();) {
			// get the target and its variable domain
			data::Target* target = context.Targets().LookupOrCreate(it.Next());
			data::VariableDomain* domain = target->Variables(true);

			// set the variables
			for (StringList::Iterator varIt = lhs.GetIterator();
					varIt.HasNext();) {
				String variable = varIt.Next();
				switch (fOperator) {
					case ASSIGNMENT_OPERATOR_ASSIGN:
						domain->Set(variable, rhs);
						break;
					case ASSIGNMENT_OPERATOR_APPEND:
						domain->LookupOrCreate(variable).Append(rhs);
						break;
					case ASSIGNMENT_OPERATOR_DEFAULT:
						if (domain->Lookup(variable) == NULL)
							domain->Set(variable, rhs);
						break;
				}
			}
		}
	} else {
		// No targets. If a local variable with the respective name exists, we
		// set it, otherwise we set one in a global scope.
		for (StringList::Iterator it = lhs.GetIterator(); it.HasNext();) {
			String variable = it.Next();

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
					data->Append(rhs);
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


}	// namespace code
}	// namespace ham
