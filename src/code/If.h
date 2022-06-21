/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_IF_H
#define HAM_CODE_IF_H

#include "code/Node.h"

namespace ham
{
namespace code
{

class If : public Node
{
  public:
	If(Node* expression, Node* block, Node* elseBlock = nullptr);
	virtual ~If();

	virtual StringList Evaluate(EvaluationContext& context);
	virtual Node* Visit(NodeVisitor& visitor);
	virtual void Dump(DumpContext& context) const;

  private:
	Node* fExpression;
	Node* fBlock;
	Node* fElseBlock;
};

} // namespace code
} // namespace ham

#endif // HAM_CODE_IF_H
