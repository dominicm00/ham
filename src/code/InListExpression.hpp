/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_IN_LIST_EXPRESSION_HPP
#define HAM_CODE_IN_LIST_EXPRESSION_HPP

#include "code/Node.hpp"

namespace ham
{
namespace code
{

class InListExpression : public Node
{
  public:
	InListExpression(Node* left, Node* right);
	virtual ~InListExpression();

	virtual StringList Evaluate(EvaluationContext& context);
	virtual Node* Visit(NodeVisitor& visitor);
	virtual void Dump(DumpContext& context) const;

  private:
	Node* fLeft;
	Node* fRight;
};

} // namespace code
} // namespace ham

#endif // HAM_CODE_IN_LIST_EXPRESSION_HPP
