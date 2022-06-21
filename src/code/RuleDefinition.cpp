/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "code/RuleDefinition.h"

#include "code/Block.h"
#include "code/DumpContext.h"
#include "code/EvaluationContext.h"
#include "code/RulePool.h"
#include "code/UserRuleInstructions.h"

namespace ham
{
namespace code
{

RuleDefinition::RuleDefinition(const String& ruleName,
							   const StringList& parameterNames,
							   Block* block)
	: fRuleName(ruleName),
	  fParameterNames(parameterNames),
	  fBlock(block),
	  fInstructions(nullptr)
{
	fBlock->AcquireReference();

	// UserRuleInstructions::Evaluate() already sets up a new local variable
	// scope, so the block doesn't need to do that.
	fBlock->SetLocalVariableScopeNeeded(false);

	fInstructions = new UserRuleInstructions(fParameterNames, fBlock);
}

RuleDefinition::~RuleDefinition()
{
	fInstructions->ReleaseReference();
	fBlock->ReleaseReference();
}

StringList
RuleDefinition::Evaluate(EvaluationContext& context)
{
	Rule& rule = context.Rules().LookupOrCreate(fRuleName);
	rule.SetInstructions(fInstructions);
	return StringList::False();
}

code::Node*
RuleDefinition::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	return fBlock->Visit(visitor);
}

void
RuleDefinition::Dump(DumpContext& context) const
{
	context << "RuleDefinition(\"" << fRuleName << "\", " << fParameterNames
			<< ")\n";

	context.BeginChildren();

	fBlock->Dump(context);

	context.EndChildren();
	context << ")\n";
}

} // namespace code
} // namespace ham
