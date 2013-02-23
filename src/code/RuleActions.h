/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_RULE_ACTIONS_H
#define HAM_CODE_RULE_ACTIONS_H


#include "data/StringList.h"


namespace ham {
namespace code {


class EvaluationContext;


class RuleActions {
public:
		enum {
			UPDATED			= 0x01,
			TOGETHER		= 0x02,
			IGNORE			= 0x04,
			QUIETLY			= 0x08,
			PIECEMEAL		= 0x10,
			EXISTING		= 0x20,
			FLAG_MASK		= 0x3f,
			MAX_LINE_FACTOR	= 0x40
		};

public:
								RuleActions(const StringList& variables,
									const String& actions, uint32_t flags);
								~RuleActions();

private:
			StringList			fVariables;
			String				fActions;
			uint32_t			fFlags;
};


}	// namespace code
}	// namespace ham


#endif	// HAM_CODE_RULE_ACTIONS_H
