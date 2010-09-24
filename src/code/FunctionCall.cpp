/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/FunctionCall.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


using code::FunctionCall;


FunctionCall::FunctionCall(Node* function)
	:
	fFunction(function),
	fArguments()
{
}

FunctionCall::FunctionCall(Node* function, const ListList& arguments)
	:
	fFunction(function),
	fArguments(arguments)
{
}

StringList
FunctionCall::Evaluate(EvaluationContext& context)
{
//		StringList functions = fFunction->Evaluate(context);
// TODO: Call each function. Concatenate the results.
	return kFalseStringList;
}

void
FunctionCall::Dump(DumpContext& context) const
{
	context << "FunctionCall(\n";
	context.BeginChildren();

	fFunction->Dump(context);

	for (ArgumentList::const_iterator it = fArguments.begin();
		it != fArguments.end(); ++it) {
		(*it)->Dump(context);
	}

	context.EndChildren();
	context << ")\n";
}
