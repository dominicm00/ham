/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_BLOCK_HPP
#define HAM_CODE_BLOCK_HPP

#include "code/Node.hpp"

#include <list>

namespace ham::code
{

class Block : public Node
{
  public:
	Block();
	virtual ~Block();

	inline void SetLocalVariableScopeNeeded(bool localVariableScopeNeeded);

	inline void AppendKeepReference(Node* statement);

	virtual StringList Evaluate(EvaluationContext& context);
	virtual Node* Visit(NodeVisitor& visitor);
	virtual void Dump(DumpContext& context) const;

  private:
	StringList _Evaluate(EvaluationContext& context);

  private:
	typedef std::list<Node*> StatementList;

	StatementList fStatements;
	bool fLocalVariableScopeNeeded;
};

void
Block::SetLocalVariableScopeNeeded(bool localVariableScopeNeeded)
{
	fLocalVariableScopeNeeded = localVariableScopeNeeded;
}

void
Block::AppendKeepReference(Node* statement)
{
	try {
		fStatements.push_back(statement);
	} catch (...) {
		statement->ReleaseReference();
	}
}

} // namespace ham::code

#endif // HAM_CODE_BLOCK_HPP
