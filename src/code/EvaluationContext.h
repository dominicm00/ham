/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_EVALUATION_CONTEXT_H
#define HAM_CODE_EVALUATION_CONTEXT_H


#include <ostream>

#include "behavior/Behavior.h"
#include "code/Defs.h"
#include "code/RulePool.h"
#include "data/VariableScope.h"


namespace ham {


namespace data {
	class TargetPool;
}


namespace code {


class EvaluationContext {
public:
								EvaluationContext(
									data::VariableDomain& globalVariables,
									data::TargetPool& targets);

			behavior::Compatibility	GetCompatibility() const
									{ return fCompatibility; }
			void				SetCompatibility(
									behavior::Compatibility compatibility);

			behavior::Behavior	GetBehavior() const
									{ return fBehavior; }
			void				SetBehavior(behavior::Behavior behavior)
									{ fBehavior = behavior; }

			data::VariableDomain* GlobalVariables()
									{ return &fGlobalVariables; }
			data::VariableScope* GlobalScope()
									{ return &fGlobalScope; }

			data::VariableScope* LocalScope() const
									{ return fLocalScope; }
			void				SetLocalScope(data::VariableScope* scope)
									{ fLocalScope = scope; }

			data::VariableDomain* BuiltInVariables() const
									{ return fBuiltInVariables; }
			void				SetBuiltInVariables(
									data::VariableDomain* variables)
									{ fBuiltInVariables = variables; }

	inline	StringList*			LookupVariable(const String& variable) const;

			data::TargetPool&	Targets() const	{ return fTargets; }

			RulePool&			Rules()			{ return fRules; }

			JumpCondition		GetJumpCondition() const
									{ return fJumpCondition; }
			void				SetJumpCondition(JumpCondition condition)
									{ fJumpCondition = condition; }

			std::ostream&		Output() const
									{ return *fOutput; }
			void				SetOutput(std::ostream& output)
									{ fOutput = &output; }
			std::ostream&		ErrorOutput() const
									{ return *fErrorOutput; }
			void				SetErrorOutput(std::ostream& output)
									{ fErrorOutput = &output; }

private:
			behavior::Compatibility fCompatibility;
			behavior::Behavior	fBehavior;
			data::VariableDomain& fGlobalVariables;
			data::VariableScope	fGlobalScope;
			data::VariableScope* fLocalScope;
			data::VariableDomain* fBuiltInVariables;
			data::TargetPool&	fTargets;
			RulePool			fRules;
			JumpCondition		fJumpCondition;
			std::ostream*		fOutput;
			std::ostream*		fErrorOutput;
};


inline StringList*
EvaluationContext::LookupVariable(const String& variable) const
{
	if (fBuiltInVariables != NULL) {
		if (StringList* result = fBuiltInVariables->Lookup(variable))
			return result;
	}

	if (fLocalScope != NULL) {
		if (StringList* result = fLocalScope->Lookup(variable))
			return result;
	}

	return fGlobalScope.Lookup(variable);
}


}	// namespace code
}	// namespace ham


#endif	// HAM_CODE_EVALUATION_CONTEXT_H
