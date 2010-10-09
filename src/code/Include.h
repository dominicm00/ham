/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_INCLUDE_H
#define HAM_CODE_INCLUDE_H


#include "code/Node.h"


namespace code {


class Include : public Node {
public:
								Include(Node* fileNames);
	virtual						~Include();

	virtual	StringList			Evaluate(EvaluationContext& context);
	virtual	Node*				Visit(NodeVisitor& visitor);
	virtual	void				Dump(DumpContext& context) const;

private:
			Node*				fFileNames;
};


}	// namespace code


#endif	// HAM_CODE_INCLUDE_H
