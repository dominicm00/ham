/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_BUILT_IN_RULES_H
#define HAM_CODE_BUILT_IN_RULES_H


namespace ham {
namespace code {


class RuleInstructions;
class RulePool;


class BuiltInRules {
public:
	static	void				RegisterRules(RulePool& rulePool);

private:
	static	void				_AddRule(RulePool& rulePool, const char* name,
									RuleInstructions* instructions);
};


}	// namespace code
}	// namespace ham


#endif // HAM_CODE_BUILT_IN_RULES_H
