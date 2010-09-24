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


StringList
Switch::Evaluate(EvaluationContext& context)
{
// TODO: Implement!
	return kFalseStringList;
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
