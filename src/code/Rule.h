/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_RULE_H
#define HAM_CODE_RULE_H


#include "code/RuleInstructions.h"
#include "data/String.h"


namespace code {


class RuleInstructions;
class RuleActions;


using data::String;


class Rule {
public:
	inline						Rule();
	inline						~Rule();

			void				SetName(const String& name)
									{ fName = name; }

			RuleInstructions*	Instructions() const
									{ return fInstructions; }
	inline	void				SetInstructions(RuleInstructions* instructions);

			RuleActions*		Actions() const
									{ return fActions; }
	inline	void				SetActions(RuleActions* actions);

private:
			String				fName;
			RuleInstructions*	fInstructions;
			RuleActions*		fActions;
};


Rule::Rule()
	:
	fName(),
	fInstructions(NULL),
	fActions(NULL)
{
}


Rule::~Rule()
{
	delete fInstructions;
// TODO:
//	delete fActions;
}


void
Rule::SetInstructions(RuleInstructions* instructions)
{
	delete fInstructions;
	fInstructions = instructions;
}


void
Rule::SetActions(RuleActions* actions)
{
// TODO:
//	delete fActions;
	fActions = actions;
}


}	// namespace code


#endif	// HAM_CODE_RULE_H
