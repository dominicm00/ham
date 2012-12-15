/*
 * Copyright 2010-2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_EVALUATION_CONTEXT_H
#define HAM_CODE_EVALUATION_CONTEXT_H


#include <ostream>

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

			data::VariableDomain* GlobalVariables()
									{ return &fGlobalVariables; }
			data::VariableScope* GlobalScope() const
									{ return fGlobalScope; }
			void				SetGlobalScope(data::VariableScope* scope)
									{ fGlobalScope = scope; }

			data::VariableScope* LocalScope() const
									{ return fLocalScope; }
			void				SetLocalScope(data::VariableScope* scope)
									{ fLocalScope = scope; }

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
			data::VariableDomain& fGlobalVariables;
			data::VariableScope	fRootScope;
			data::VariableScope* fGlobalScope;
			data::VariableScope* fLocalScope;
			data::TargetPool&	fTargets;
			RulePool			fRules;
			JumpCondition		fJumpCondition;
			std::ostream*		fOutput;
			std::ostream*		fErrorOutput;
};


inline StringList*
EvaluationContext::LookupVariable(const String& variable) const
{
	StringList* result = NULL;
	if (fLocalScope != NULL)
		result = fLocalScope->Lookup(variable);
	if (result == NULL)
		result = fGlobalScope->Lookup(variable);
	return result;
}


}	// namespace code
}	// namespace ham


#endif	// HAM_CODE_EVALUATION_CONTEXT_H
