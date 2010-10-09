/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_SWITCH_H
#define HAM_CODE_SWITCH_H


#include <list>

#include "code/Case.h"


namespace code {


class Switch : public Node {
public:
								Switch(Node* argument);
	virtual						~Switch();

	inline	void				AddCase(Case* caseStatement);

	virtual	StringList			Evaluate(EvaluationContext& context);
	virtual	Node*				Visit(NodeVisitor& visitor);
	virtual	void				Dump(DumpContext& context) const;

private:
			typedef std::list<Case*> CaseList;

			Node*				fArgument;
			CaseList			fCases;
};


void
Switch::AddCase(Case* caseStatement)
{
	fCases.push_back(caseStatement);
}


}	// namespace code


#endif	// HAM_CODE_SWITCH_H
