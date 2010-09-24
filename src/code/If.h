/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_IF_H
#define HAM_CODE_IF_H


#include "code/Node.h"


namespace code {


class If : public Node {
public:
								If(Node* expression, Node* block);

	inline	void				SetElseBlock(Node* block);

	virtual	StringList			Evaluate(EvaluationContext& context);

	virtual	void				Dump(DumpContext& context) const;

private:
			Node*				fExpression;
			Node*				fBlock;
			Node*				fElseBlock;
};


void
If::SetElseBlock(Node* block)
{
	fElseBlock = block;
}


}	// namespace code


#endif	// HAM_CODE_IF_H
