/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_BUILT_IN_RULES_H
#define HAM_CODE_BUILT_IN_RULES_H


#include <stddef.h>


namespace ham {
namespace code {


class RuleInstructions;
class RulePool;


class BuiltInRules {
public:
	static	void				RegisterRules(RulePool& rulePool);

private:
	static	void				_AddRuleConsumeReference(RulePool& rulePool,
									const char* name,
									RuleInstructions* instructions,
									const char* alias1 = NULL,
									const char* alias2 = NULL);
									// consumes instructions reference
	static inline void			_AddRule(RulePool& rulePool, const char* name,
									RuleInstructions* instructions);
									// caller keeps instructions reference
};


}	// namespace code
}	// namespace ham


#endif // HAM_CODE_BUILT_IN_RULES_H
