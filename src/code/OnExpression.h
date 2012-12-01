/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_ON_EXPRESSION_H
#define HAM_CODE_ON_EXPRESSION_H


#include "code/Node.h"


namespace ham {
namespace code {


class OnExpression : public Node {
public:
								OnExpression(Node* object, Node* expression);
	virtual						~OnExpression();

	virtual	StringList			Evaluate(EvaluationContext& context);
	virtual	Node*				Visit(NodeVisitor& visitor);
	virtual	void				Dump(DumpContext& context) const;

private:
			Node*				fObject;
			Node*				fExpression;
};


}	// namespace code
}	// namespace ham


#endif	// HAM_CODE_ON_EXPRESSION_H
