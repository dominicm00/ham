/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_LIST_H
#define HAM_CODE_LIST_H


#include <vector>

#include "code/Node.h"


namespace ham {
namespace code {


class List : public Node {
public:
								List();
	virtual						~List();

	inline	List&				operator+=(Node* child);

	virtual	StringList			Evaluate(EvaluationContext& context);
	virtual	Node*				Visit(NodeVisitor& visitor);
	virtual	void				Dump(DumpContext& context) const;

private:
			std::vector<Node*>	fChildren;
};


List&
List::operator+=(Node* child)
{
	try {
		fChildren.push_back(child);
		return *this;
	} catch (...) {
		delete child;
		throw;
	}
}


}	// namespace code
}	// namespace ham


#endif	// HAM_CODE_LIST_H
