/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_RULE_ACTIONS_HPP
#define HAM_DATA_RULE_ACTIONS_HPP

#include "data/StringList.hpp"
#include "data/TargetContainers.hpp"
#include "util/Referenceable.hpp"

namespace ham::data
{

class EvaluationContext;

class RuleActions : public util::Referenceable
{
  public:
	enum {
		UPDATED = 0x01,
		TOGETHER = 0x02,
		IGNORE = 0x04,
		QUIETLY = 0x08,
		PIECEMEAL = 0x10,
		EXISTING = 0x20,
		FLAG_MASK = 0x3f,
		MAX_LINE_FACTOR = 0x40
	};

  public:
	RuleActions(const String& ruleName,
				const StringList& variables,
				const String& actions,
				uint32_t flags);
	~RuleActions();

	const String& RuleName() const { return fRuleName; }
	const StringList& Variables() const { return fVariables; }
	const String& Actions() const { return fActions; }
	uint32_t Flags() const { return fFlags; }

  private:
	String fRuleName;
	StringList fVariables;
	String fActions;
	uint32_t fFlags;
};

class RuleActionsCall : public util::Referenceable
{
  public:
	RuleActionsCall(RuleActions* actions,
					const TargetList& targets,
					const TargetList& sourceTargets)
		: fActions(actions),
		  fTargets(targets),
		  fSourceTargets(sourceTargets)
	{
		fActions->AcquireReference();
	}

	RuleActionsCall(const RuleActionsCall& other)
		: fActions(other.fActions),
		  fTargets(other.fTargets),
		  fSourceTargets(other.fSourceTargets)
	{
		fActions->AcquireReference();
	}

	~RuleActionsCall() { fActions->ReleaseReference(); }

	RuleActions* Actions() const { return fActions; }

	const TargetList& Targets() const { return fTargets; }

	const TargetList& SourceTargets() const { return fSourceTargets; }

	RuleActionsCall& operator=(const RuleActionsCall& other)
	{
		fActions->ReleaseReference();
		fActions = other.fActions;
		fActions->AcquireReference();
		fTargets = other.fTargets;
		fSourceTargets = other.fSourceTargets;
		return *this;
	}

  private:
	RuleActions* fActions;
	TargetList fTargets;
	TargetList fSourceTargets;
};

} // namespace ham::data

#endif // HAM_DATA_RULE_ACTIONS_HPP
