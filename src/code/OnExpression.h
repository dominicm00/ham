/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_ON_EXPRESSION_H
#define HAM_CODE_ON_EXPRESSION_H


#include "code/Node.h"


namespace code {


class OnExpression : public Node {
public:
								OnExpression(Node* object);
								OnExpression(Node* object, Node* expression);

	inline	void				SetExpression(Node* expression);

	virtual	StringList			Evaluate(EvaluationContext& context);

	virtual	void				Dump(DumpContext& context) const;

private:
			Node*				fObject;
			Node*				fExpression;
};


void
OnExpression::SetExpression(Node* expression)
{
	fExpression = expression;
}


}	// namespace code


#endif	// HAM_CODE_ON_EXPRESSION_H
