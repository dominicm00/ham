/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_RULE_H
#define HAM_CODE_RULE_H

#include "code/RuleInstructions.h"
#include "data/RuleActions.h"
#include "data/String.h"

namespace ham
{
namespace code
{

using data::String;

class Rule
{
  public:
	inline Rule();
	inline ~Rule();

	void SetName(const String& name) { fName = name; }

	RuleInstructions* Instructions() const { return fInstructions; }
	inline void SetInstructions(RuleInstructions* instructions);

	data::RuleActions* Actions() const { return fActions; }
	inline void SetActions(data::RuleActions* actions);
	// takes over ownership

  private:
	String fName;
	RuleInstructions* fInstructions;
	data::RuleActions* fActions;
};

Rule::Rule()
	: fName(),
	  fInstructions(NULL),
	  fActions(NULL)
{
}

Rule::~Rule()
{
	if (fInstructions != NULL)
		fInstructions->ReleaseReference();
	if (fActions != NULL)
		fActions->ReleaseReference();
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
Rule::SetActions(data::RuleActions* actions)
{
	if (fActions != NULL)
		fActions->ReleaseReference();

	fActions = actions;

	if (fActions != NULL)
		fActions->AcquireReference();
}

} // namespace code
} // namespace ham

#endif // HAM_CODE_RULE_H
