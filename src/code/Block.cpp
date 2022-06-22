/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "code/Block.hpp"

#include "code/DumpContext.hpp"
#include "code/EvaluationContext.hpp"

namespace ham
{
namespace code
{

Block::Block()
	: fStatements(),
	  fLocalVariableScopeNeeded(true)
{
}

Block::~Block()
{
	for (StatementList::const_iterator it = fStatements.begin();
		 it != fStatements.end();
		 ++it) {
		(*it)->ReleaseReference();
	}
}

StringList
Block::Evaluate(EvaluationContext& context)
{
	if (!fLocalVariableScopeNeeded)
		return _Evaluate(context);

	// Push a fresh local variable scope. It inherits the old one, so, unless
	// shadowed, already defined local variables can still be seen.
	data::VariableScope* oldLocalScope = context.LocalScope();
	data::VariableDomain localVariables;
	data::VariableScope localScope(localVariables, oldLocalScope);
	if (fLocalVariableScopeNeeded)
		context.SetLocalScope(&localScope);

	StringList result = _Evaluate(context);

	// reinstate the old local variable scope
	if (fLocalVariableScopeNeeded)
		context.SetLocalScope(oldLocalScope);

	return result;
}

code::Node*
Block::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	for (StatementList::const_iterator it = fStatements.begin();
		 it != fStatements.end();
		 ++it) {
		if (Node* result = (*it)->Visit(visitor))
			return result;
	}

	return nullptr;
}

void
Block::Dump(DumpContext& context) const
{
	context << "Block(\n";
	context.BeginChildren();

	for (StatementList::const_iterator it = fStatements.begin();
		 it != fStatements.end();
		 ++it) {
		(*it)->Dump(context);
	}

	context.EndChildren();
	context << ")\n";
}

StringList
Block::_Evaluate(EvaluationContext& context)
{
	StringList result;
	for (StatementList::const_iterator it = fStatements.begin();
		 it != fStatements.end();
		 ++it) {
		result = (*it)->Evaluate(context);

		// terminate the block early, if a jump condition is set
		if (context.GetJumpCondition() != JUMP_CONDITION_NONE)
			break;
	}

	return result;
}

} // namespace code
} // namespace ham
