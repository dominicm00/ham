/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_RULE_H
#define HAM_CODE_RULE_H


#include "code/RuleActions.h"
#include "code/RuleInstructions.h"
#include "data/String.h"


namespace ham {
namespace code {


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
									// takes over ownership

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
	if (fInstructions != NULL)
		fInstructions->ReleaseReference();
	delete fActions;
}


void
Rule::SetInstructions(RuleInstructions* instructions)
{
	if (fInstructions != NULL)
		fInstructions->ReleaseReference();

	fInstructions = instructions;

	if (fInstructions != NULL)
		fInstructions->AcquireReference();
}


void
Rule::SetActions(RuleActions* actions)
{
	delete fActions;
	fActions = actions;
}


}	// namespace code
}	// namespace ham


#endif	// HAM_CODE_RULE_H
