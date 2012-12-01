/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_BINARY_EXPRESSION_H
#define HAM_CODE_BINARY_EXPRESSION_H


#include "code/Node.h"


namespace ham {
namespace code {


template<typename Operator>
class BinaryExpression : public Node {
public:
								BinaryExpression(Node* left, Node* right);
	virtual						~BinaryExpression();

	virtual	StringList			Evaluate(EvaluationContext& context);
	virtual	Node*				Visit(NodeVisitor& visitor);
	virtual	void				Dump(DumpContext& context) const;

private:
			Node*				fLeft;
			Node*				fRight;
};


// declare the specializations

#define HAM_DECLARE_OPERATOR_EXPRESSION(name)					\
	struct name##Operator;										\
	typedef BinaryExpression<name##Operator> name##Expression;

HAM_DECLARE_OPERATOR_EXPRESSION(Equal)
HAM_DECLARE_OPERATOR_EXPRESSION(NotEqual)
HAM_DECLARE_OPERATOR_EXPRESSION(Less)
HAM_DECLARE_OPERATOR_EXPRESSION(LessOrEqual)
HAM_DECLARE_OPERATOR_EXPRESSION(Greater)
HAM_DECLARE_OPERATOR_EXPRESSION(GreaterOrEqual)
HAM_DECLARE_OPERATOR_EXPRESSION(And)
HAM_DECLARE_OPERATOR_EXPRESSION(Or)

#undef HAM_DECLARE_OPERATOR_EXPRESSION


}	// namespace code
}	// namespace ham


#endif	// HAM_CODE_BINARY_EXPRESSION_H
