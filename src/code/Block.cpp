/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/Block.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


using code::Block;


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
	for (StatementList::const_iterator it = fStatements.begin();
			it != fStatements.end(); ++it) {
		/*StringList result =*/ (*it)->Evaluate(context);
		// TODO: In case of break/continue/return, return here.
	}

	return kFalseStringList;
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
