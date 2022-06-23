/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_USER_RULE_INSTRUCTIONS_HPP
#define HAM_CODE_USER_RULE_INSTRUCTIONS_HPP

#include "code/RuleInstructions.hpp"

namespace ham::code
{

class Node;

class UserRuleInstructions : public RuleInstructions
{
  public:
	UserRuleInstructions(const StringList& parameterNames, Node* block);
	~UserRuleInstructions();

	virtual StringList
	Evaluate(EvaluationContext& context, const StringListList& parameters);

  private:
	StringList fParameterNames;
	Node* fBlock;
};

} // namespace ham::code

#endif // HAM_CODE_USER_RULE_INSTRUCTIONS_HPP
