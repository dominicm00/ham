/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_SWITCH_HPP
#define HAM_CODE_SWITCH_HPP

#include "code/Case.hpp"

#include <list>

namespace ham::code
{

class Switch : public Node
{
  public:
	Switch(Node* argument);
	virtual ~Switch();

	inline void AddCase(const String& pattern, Node* block);

	virtual StringList Evaluate(EvaluationContext& context);
	virtual Node* Visit(NodeVisitor& visitor);
	virtual void Dump(DumpContext& context) const;

  private:
	typedef std::list<Case*> CaseList;

	Node* fArgument;
	CaseList fCases;
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

} // namespace ham::code

#endif // HAM_CODE_SWITCH_HPP
