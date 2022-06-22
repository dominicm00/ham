/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_NOT_EXPRESSION_HPP
#define HAM_CODE_NOT_EXPRESSION_HPP

#include "code/Node.hpp"

namespace ham::code
{

class NotExpression : public Node
{
  public:
	NotExpression(Node* child);
	virtual ~NotExpression();

	virtual StringList Evaluate(EvaluationContext& context);
	virtual Node* Visit(NodeVisitor& visitor);
	virtual void Dump(DumpContext& context) const;

  private:
	Node* fChild;
};

} // namespace ham::code

#endif // HAM_CODE_NOT_EXPRESSION_HPP
