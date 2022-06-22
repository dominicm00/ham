/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_RULE_HPP
#define HAM_CODE_RULE_HPP

#include "code/RuleInstructions.hpp"
#include "data/RuleActions.hpp"
#include "data/String.hpp"

namespace ham::code
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
	  fInstructions(nullptr),
	  fActions(nullptr)
{
}

Rule::~Rule()
{
	if (fInstructions != nullptr)
		fInstructions->ReleaseReference();
	if (fActions != nullptr)
		fActions->ReleaseReference();
}

void
Rule::SetInstructions(RuleInstructions* instructions)
{
	if (fInstructions != nullptr)
		fInstructions->ReleaseReference();

	fInstructions = instructions;

	if (fInstructions != nullptr)
		fInstructions->AcquireReference();
}

void
Rule::SetActions(data::RuleActions* actions)
{
	if (fActions != nullptr)
		fActions->ReleaseReference();

	fActions = actions;

	if (fActions != nullptr)
		fActions->AcquireReference();
}

} // namespace ham::code

#endif // HAM_CODE_RULE_HPP
