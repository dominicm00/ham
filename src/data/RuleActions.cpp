/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "data/RuleActions.hpp"

namespace ham
{
namespace data
{

RuleActions::RuleActions(const String& ruleName,
						 const StringList& variables,
						 const String& actions,
						 uint32_t flags)
	: util::Referenceable(),
	  fRuleName(ruleName),
	  fVariables(variables),
	  fActions(actions),
	  fFlags(flags)
{
}

RuleActions::~RuleActions() {}

} // namespace data
} // namespace ham
