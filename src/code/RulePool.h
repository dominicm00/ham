/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_RULE_POOL_H
#define HAM_CODE_RULE_POOL_H


#include <map>

#include "code/Rule.h"


namespace code {


class RuleInstructions;
class RuleActions;


class RulePool {
public:
								RulePool()	{}
								~RulePool()	{}

	inline	Rule*				Lookup(const String& name);
	inline	Rule&				LookupOrCreate(const String& name);

private:
			typedef std::map<String, Rule> RuleMap;

private:
			RuleMap				fRules;
};


Rule*
RulePool::Lookup(const String& name)
{
	RuleMap::iterator it = fRules.find(name);
	return it == fRules.end() ? NULL : &it->second;
}


Rule&
RulePool::LookupOrCreate(const String& name)
{
	// look up the rule
	RuleMap::iterator it = fRules.find(name);
	if (it != fRules.end())
		return it->second;

	// no rule yet -- create one
	Rule& rule = fRules[name];
	rule.SetName(name);
	return rule;
}


}	// namespace code


#endif	// HAM_CODE_RULE_POOL_H
