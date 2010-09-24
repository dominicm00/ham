/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_WHILE_H
#define HAM_CODE_WHILE_H


#include "code/Node.h"


namespace code {


class While : public Node {
public:
								While(Node* expression, Node* block);

	virtual	StringList			Evaluate(EvaluationContext& context);

	virtual	void				Dump(DumpContext& context) const;

private:
			Node*				fExpression;
			Node*				fBlock;
};


}	// namespace code


#endif	// HAM_CODE_WHILE_H
