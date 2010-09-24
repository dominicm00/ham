/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_BLOCK_H
#define HAM_CODE_BLOCK_H


#include <list>

#include "code/Node.h"


namespace code {


class Block : public Node {
public:
								Block();

	virtual	StringList			Evaluate(EvaluationContext& context);

	virtual	void				Dump(DumpContext& context) const;

	inline	Block&				operator+=(Node* statement);

private:
			typedef std::list<Node*> StatementList;

			StatementList		fStatements;
};


Block&
Block::operator+=(Node* statement)
{
	fStatements.push_back(statement);
	return *this;
}


}	// namespace code


#endif	// HAM_CODE_BLOCK_H
