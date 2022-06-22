/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_RULE_INSTRUCTIONS_HPP
#define HAM_CODE_RULE_INSTRUCTIONS_HPP

#include "data/StringList.hpp"
#include "util/Referenceable.hpp"

namespace ham::code
{

class EvaluationContext;

class RuleInstructions : public util::Referenceable
{
  public:
	virtual ~RuleInstructions();

	virtual StringList Evaluate(EvaluationContext& context,
								const StringListList& parameters) = 0;
};

} // namespace ham::code

#endif // HAM_CODE_RULE_INSTRUCTIONS_HPP
