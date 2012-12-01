/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/FunctionCall.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


namespace ham {
namespace code {


FunctionCall::FunctionCall(Node* function)
	:
	fFunction(function),
	fArguments()
{
}


FunctionCall::FunctionCall(Node* function, const NodeList& arguments)
	:
	fFunction(function),
	fArguments(arguments)
{
}


FunctionCall::~FunctionCall()
{
	delete fFunction;
}


StringList
FunctionCall::Evaluate(EvaluationContext& context)
{
//		StringList functions = fFunction->Evaluate(context);
// TODO: Call each function. Concatenate the results.
	return data::kFalseStringList;
}


code::Node*
FunctionCall::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	if (Node* result = fFunction->Visit(visitor))
		return result;

	for (ArgumentList::const_iterator it = fArguments.begin();
			it != fArguments.end(); ++it) {
		if (Node* result = (*it)->Visit(visitor))
			return result;
	}

	return NULL;
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


}	// namespace code
}	// namespace ham
