/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_RULE_INSTRUCTIONS_H
#define HAM_CODE_RULE_INSTRUCTIONS_H


#include "data/StringList.h"


namespace ham {
namespace code {


class EvaluationContext;


class RuleInstructions {
public:
	virtual						~RuleInstructions();

	virtual	StringList			Evaluate(EvaluationContext& context,
									const StringListList& parameters) = 0;
};


}	// namespace code
}	// namespace ham


#endif	// HAM_CODE_RULE_INSTRUCTIONS_H
