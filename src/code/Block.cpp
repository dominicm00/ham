/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/Block.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


namespace ham {
namespace code {


Block::Block()
	:
	fStatements()
{
}


Block::~Block()
{
	for (StatementList::const_iterator it = fStatements.begin();
			it != fStatements.end(); ++it) {
		delete *it;
	}
}


StringList
Block::Evaluate(EvaluationContext& context)
{
	StringList result;
	for (StatementList::const_iterator it = fStatements.begin();
			it != fStatements.end(); ++it) {
		result = (*it)->Evaluate(context);

		// terminate the block early, if a jump condition is set
		if (context.GetJumpCondition() != JUMP_CONDITION_NONE)
			break;
	}

	return result;
}


code::Node*
Block::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	for (StatementList::const_iterator it = fStatements.begin();
			it != fStatements.end(); ++it) {
		if (Node* result = (*it)->Visit(visitor))
			return result;
	}

	return NULL;
}


void
Block::Dump(DumpContext& context) const
{
	context << "Block(\n";
	context.BeginChildren();

	for (StatementList::const_iterator it = fStatements.begin();
			it != fStatements.end(); ++it) {
		(*it)->Dump(context);
	}

	context.EndChildren();
	context << ")\n";
}


}	// namespace code
}	// namespace ham
