/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_LIST_H
#define HAM_CODE_LIST_H


#include <vector>

#include "code/Node.h"


namespace code {


class List : public Node {
public:
								List();

	inline	List&				operator+=(Node* child);

	virtual	StringList			Evaluate(EvaluationContext& context);

	virtual	void				Dump(DumpContext& context) const;

private:
			std::vector<Node*>	fChildren;
};


typedef std::list<List*> ListList;


List&
List::operator+=(Node* child)
{
	fChildren.push_back(child);
	return *this;
}


}	// namespace code


#endif	// HAM_CODE_LIST_H
