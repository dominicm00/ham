/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_EVALUATION_CONTEXT_HPP
#define HAM_CODE_EVALUATION_CONTEXT_HPP

#include "behavior/Behavior.hpp"
#include "code/Defs.hpp"
#include "code/RulePool.hpp"
#include "data/VariableScope.hpp"

#include <ostream>

namespace ham
{

namespace data
{
class TargetPool;
}

namespace code
{

/**
 * Complete context where variables are evaluated.
 */
class EvaluationContext
{
  public:
	// XXX: Giving compatibility and behavior default values makes it easy to
	// end up with inconsistent compatibility behavior. This constructor should
	// probably require a compatibility value.
	EvaluationContext(data::VariableDomain& globalVariables,
					  data::TargetPool& targets);

	behavior::Compatibility GetCompatibility() const { return fCompatibility; }
	void SetCompatibility(behavior::Compatibility compatibility);
	// resets behavior as well

	behavior::Behavior GetBehavior() const { return fBehavior; }
	void SetBehavior(behavior::Behavior behavior) { fBehavior = behavior; }

	data::VariableDomain* GlobalVariables() { return &fGlobalVariables; }

	data::VariableScope* LocalScope() const { return fLocalScope; }
	void SetLocalScope(data::VariableScope* scope) { fLocalScope = scope; }

	data::VariableDomain* BuiltInVariables() const { return fBuiltInVariables; }
	void SetBuiltInVariables(data::VariableDomain* variables)
	{
		fBuiltInVariables = variables;
	}

	inline const StringList* LookupVariable(const String& variable) const;

	data::TargetPool& Targets() const { return fTargets; }

	RulePool& Rules() { return fRules; }

	JumpCondition GetJumpCondition() const { return fJumpCondition; }
	void SetJumpCondition(JumpCondition condition)
	{
		fJumpCondition = condition;
	}

	size_t IncludeDepth() const { return fIncludeDepth; }
	void SetIncludeDepth(size_t depth) { fIncludeDepth = depth; }

	size_t RuleCallDepth() const { return fRuleCallDepth; }
	void SetRuleCallDepth(size_t depth) { fRuleCallDepth = depth; }

	std::ostream& Output() const { return *fOutput; }
	void SetOutput(std::ostream& output) { fOutput = &output; }
	std::ostream& ErrorOutput() const { return *fErrorOutput; }
	void SetErrorOutput(std::ostream& output) { fErrorOutput = &output; }

  private:
	behavior::Compatibility fCompatibility;
	behavior::Behavior fBehavior;
	data::VariableDomain& fGlobalVariables;
	data::VariableScope* fLocalScope;
	data::VariableDomain* fBuiltInVariables;
	data::TargetPool& fTargets;
	RulePool fRules;
	JumpCondition fJumpCondition;
	size_t fIncludeDepth;
	size_t fRuleCallDepth;
	std::ostream* fOutput;
	std::ostream* fErrorOutput;
};

inline const StringList*
EvaluationContext::LookupVariable(const String& variable) const
{
	if (fBuiltInVariables != nullptr) {
		if (const StringList* result = fBuiltInVariables->Lookup(variable))
			return result;
	}

	if (fLocalScope != nullptr) {
		if (const StringList* result = fLocalScope->Lookup(variable))
			return result;
	}

	return fGlobalVariables.Lookup(variable);
}

} // namespace code
} // namespace ham

#endif // HAM_CODE_EVALUATION_CONTEXT_HPP
