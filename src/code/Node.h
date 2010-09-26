/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_CODE_H
#define HAM_CODE_CODE_H


#include <list>

#include "data/String.h"


namespace code {


class DumpContext;
class EvaluationContext;


class Node {
public:
	virtual						~Node();

	virtual	StringList			Evaluate(EvaluationContext& context) = 0;
	virtual	void				Dump(DumpContext& context) const = 0;
};


typedef std::list<Node*> NodeList;


}	// namespace code


#endif	// HAM_CODE_CODE_H
