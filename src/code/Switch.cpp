/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "code/Switch.hpp"

#include "code/DumpContext.hpp"
#include "code/EvaluationContext.hpp"

namespace ham::code
{

Switch::Switch(Node* argument)
	: fArgument(argument),
	  fCases()
{
	fArgument->AcquireReference();
}

Switch::~Switch()
{
	fArgument->ReleaseReference();

	for (CaseList::const_iterator it = fCases.begin(); it != fCases.end();
		 ++it) {
		(*it)->ReleaseReference();
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
			return result;
		}
	}

	return StringList::False();
}

code::Node*
Switch::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	if (Node* result = fArgument->Visit(visitor))
		return result;

	for (CaseList::const_iterator it = fCases.begin(); it != fCases.end();
		 ++it) {
		if (Node* result = (*it)->Visit(visitor))
			return result;
	}

	return nullptr;
}

void
Switch::Dump(DumpContext& context) const
{
	context << "Switch(\n";
	context.BeginChildren();

	fArgument->Dump(context);

	for (CaseList::const_iterator it = fCases.begin(); it != fCases.end();
		 ++it) {
		(*it)->Dump(context);
	}

	context.EndChildren();
	context << ")\n";
}

} // namespace ham::code
