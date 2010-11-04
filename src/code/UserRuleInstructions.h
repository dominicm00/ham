/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_USER_RULE_INSTRUCTIONS_H
#define HAM_CODE_USER_RULE_INSTRUCTIONS_H


#include "code/RuleInstructions.h"


namespace code {


class Node;


class UserRuleInstructions : public RuleInstructions {
public:
								UserRuleInstructions(
									const StringList& parameterNames,
									Node* block);

	virtual	StringList			Evaluate(EvaluationContext& context,
									const StringList* parameters,
									size_t parameterCount);

private:
			StringList			fParameterNames;
			Node*				fBlock;
};


}	// namespace code


#endif	// HAM_CODE_USER_RULE_INSTRUCTIONS_H
