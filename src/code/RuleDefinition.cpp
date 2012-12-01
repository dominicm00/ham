/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/RuleDefinition.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"
#include "code/RulePool.h"
#include "code/UserRuleInstructions.h"


using code::RuleDefinition;


RuleDefinition::RuleDefinition(const String& ruleName)
	:
	fRuleName(ruleName),
	fParameterNames(),
	fBlock(NULL)
{
}


RuleDefinition::~RuleDefinition()
{
	delete fBlock;
}


StringList
RuleDefinition::Evaluate(EvaluationContext& context)
{
	Rule& rule = context.Rules().LookupOrCreate(fRuleName);
	rule.SetInstructions(new UserRuleInstructions(fParameterNames, fBlock));
		// TODO: Unless all node trees shall be kept in memory, we should use
		// reference counting for the block.

	return data::kFalseStringList;
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
	context << "RuleDefinition(\"" << fRuleName << "\", (";

	for (StringList::const_iterator it = fParameterNames.begin();
			it != fParameterNames.end(); ++it) {
		if (it != fParameterNames.begin())
			context << ", ";
		context << *it;
	}

	context << ")\n";

	context.BeginChildren();

	fBlock->Dump(context);

	context.EndChildren();
	context << ")\n";
}
