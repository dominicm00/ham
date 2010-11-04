/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/Switch.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


using code::Switch;


Switch::Switch(Node* argument)
	:
	fArgument(argument),
	fCases()
{
}


Switch::~Switch()
{
	delete fArgument;

	for (CaseList::const_iterator it = fCases.begin();
			it != fCases.end(); ++it) {
		delete *it;
	}
}


StringList
Switch::Evaluate(EvaluationContext& context)
{
	const StringList& argument = fArgument->Evaluate(context);

	// find the matching case
	for (CaseList::iterator it = fCases.begin(); it != fCases.end(); ++it) {
		if ((*it)->Matches(context, argument)) {
			// found match -- execute the block
			const StringList& result = (*it)->Evaluate(context);

			// clear a break jump condition
			if (context.GetJumpCondition() == JUMP_CONDITION_BREAK)
				context.SetJumpCondition(JUMP_CONDITION_NONE);

			return result;
		}
	}

	return kFalseStringList;
}


code::Node*
Switch::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	if (Node* result = fArgument->Visit(visitor))
		return result;

	for (CaseList::const_iterator it = fCases.begin();
			it != fCases.end(); ++it) {
		if (Node* result = (*it)->Visit(visitor))
			return result;
	}

	return NULL;
}


void
Switch::Dump(DumpContext& context) const
{
	context << "Switch(\n";
	context.BeginChildren();

	fArgument->Dump(context);

	for (CaseList::const_iterator it = fCases.begin();
			it != fCases.end(); ++it) {
		(*it)->Dump(context);
	}

	context.EndChildren();
	context << ")\n";
}
