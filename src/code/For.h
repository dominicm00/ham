/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_FOR_H
#define HAM_CODE_FOR_H


#include "code/Node.h"


namespace code {


class For : public Node {
public:
								For(Node* variable, Node* list,
									Node* block);

	virtual	StringList			Evaluate(EvaluationContext& context);

	virtual	void				Dump(DumpContext& context) const;

private:
			Node*				fVariable;
			Node*				fList;
			Node*				fBlock;
};


}	// namespace code


#endif	// HAM_CODE_FOR_H
