/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_RULE_INSTRUCTIONS_H
#define HAM_CODE_RULE_INSTRUCTIONS_H


#include "data/String.h"


namespace code {


class EvaluationContext;


using data::StringList;


class RuleInstructions {
public:
	virtual						~RuleInstructions();

	virtual	StringList			Evaluate(EvaluationContext& context,
									const StringList* parameters,
									size_t parameterCount) = 0;
};


}	// namespace code


#endif	// HAM_CODE_RULE_INSTRUCTIONS_H
