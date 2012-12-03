/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_CODE_H
#define HAM_CODE_CODE_H


#include <list>

#include "data/StringList.h"


namespace ham {
namespace code {


class DumpContext;
class EvaluationContext;
class Node;


class NodeVisitor {
public:
	virtual						~NodeVisitor();

	// return true to terminate visiting
	virtual	bool				VisitNode(Node* node) = 0;
};


class Node {
public:
	virtual						~Node();

	virtual	StringList			Evaluate(EvaluationContext& context) = 0;
	virtual	Node*				Visit(NodeVisitor& visitor) = 0;
	virtual	void				Dump(DumpContext& context) const = 0;
};


typedef std::list<Node*> NodeList;


}	// namespace code
}	// namespace ham


#endif	// HAM_CODE_CODE_H
