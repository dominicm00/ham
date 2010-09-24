/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_IN_LIST_EXPRESSION_H
#define HAM_CODE_IN_LIST_EXPRESSION_H


#include "code/Node.h"


namespace code {


class InListExpression : public Node {
public:
								InListExpression(Node* left, Node* right);

	virtual	StringList			Evaluate(EvaluationContext& context);

	virtual	void				Dump(DumpContext& context) const;

private:
			Node*				fLeft;
			Node*				fRight;
};


}	// namespace code


#endif	// HAM_CODE_IN_LIST_EXPRESSION_H
