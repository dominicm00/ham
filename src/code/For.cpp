/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/For.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


using code::For;


For::For(Node* variable, Node* list, Node* block)
	:
	fVariable(variable),
	fList(list),
	fBlock(block)
{
}


For::~For()
{
	delete fVariable;
	delete fList;
	delete fBlock;
}


StringList
For::Evaluate(EvaluationContext& context)
{
	// get the loop variable -- we ignore all but the first element of the
	// variable list
	const StringList& variables = fVariable->Evaluate(context);
	if (variables.empty())
		return kFalseStringList;

	// look for a local variable
	StringList* variableValue = context.LocalScope()->Lookup(variables.front());
	if (variableValue == NULL) {
		// no local variable -- check for a global one
		variableValue = context.GlobalScope()->Lookup(variables.front());
		if (variableValue == NULL) {
			// no existing global variable either -- create one
			variableValue = &context.GlobalVariables()->LookupOrCreate(
				variables.front());
		}
	}

	// perform the for loop
	StringList result;
	const StringList& list = fList->Evaluate(context);
	for (StringList::const_iterator it = list.begin(); it != list.end(); ++it) {
		// assign the variable
		variableValue->clear();
		variableValue->push_back(*it);

		// execute the block
		result = fBlock->Evaluate(context);

		// handle jump conditions
		switch (context.GetJumpCondition()) {
			case JUMP_CONDITION_NONE:
				break;
			case JUMP_CONDITION_BREAK:
				context.SetJumpCondition(JUMP_CONDITION_NONE);
				return result;
			case JUMP_CONDITION_CONTINUE:
				context.SetJumpCondition(JUMP_CONDITION_NONE);
				break;
			case JUMP_CONDITION_RETURN:
			case JUMP_CONDITION_JUMP_TO_EOF:
				return result;
		}
	}

	return result;
}


code::Node*
For::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	if (Node* result = fVariable->Visit(visitor))
		return result;

	if (Node* result = fList->Visit(visitor))
		return result;

	return fBlock->Visit(visitor);
}


void
For::Dump(DumpContext& context) const
{
	context << "For(\n";
	context.BeginChildren();

	fVariable->Dump(context);
	fList->Dump(context);
	fBlock->Dump(context);

	context.EndChildren();
	context << ")\n";
}
