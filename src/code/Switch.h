/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_SWITCH_H
#define HAM_CODE_SWITCH_H


#include <list>

#include "code/Case.h"


namespace ham {
namespace code {


class Switch : public Node {
public:
								Switch(Node* argument);
	virtual						~Switch();

	inline	void				AddCase(const String& pattern, Node* block);

	virtual	StringList			Evaluate(EvaluationContext& context);
	virtual	Node*				Visit(NodeVisitor& visitor);
	virtual	void				Dump(DumpContext& context) const;

private:
			typedef std::list<Case*> CaseList;

			Node*				fArgument;
			CaseList			fCases;
};


void
Switch::AddCase(const String& pattern, Node* block)
{
	Case* caseStatement = new Case(pattern, block);
	try {
		fCases.push_back(caseStatement);
	} catch (...) {
		delete caseStatement;
		throw;
	}
}


}	// namespace code
}	// namespace ham


#endif	// HAM_CODE_SWITCH_H
