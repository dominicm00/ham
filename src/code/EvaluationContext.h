/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_EVALUATION_CONTEXT_H
#define HAM_CODE_EVALUATION_CONTEXT_H


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

			data::TargetPool&	Targets() const	{ return fTargets; }

			RulePool&			Rules()			{ return fRules; }

			JumpCondition		GetJumpCondition() const
									{ return fJumpCondition; }
			void				SetJumpCondition(JumpCondition condition)
									{ fJumpCondition = condition; }

private:
			data::VariableDomain& fGlobalVariables;
			data::VariableScope	fRootScope;
			data::VariableScope* fGlobalScope;
			data::VariableScope* fLocalScope;
			data::TargetPool&	fTargets;
			RulePool			fRules;
			JumpCondition		fJumpCondition;
};


}	// namespace code
}	// namespace ham


#endif	// HAM_CODE_EVALUATION_CONTEXT_H
