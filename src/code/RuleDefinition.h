/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_RULE_DEFINITION_H
#define HAM_CODE_RULE_DEFINITION_H


#include "code/Node.h"


namespace ham {
namespace code {


class RuleDefinition : public Node {
public:
								RuleDefinition(const String& ruleName);
	virtual						~RuleDefinition();

	inline	void				AddParameterName(const String& parameterName);
	inline	void				SetBlock(Node* block);

	virtual	StringList			Evaluate(EvaluationContext& context);
	virtual	Node*				Visit(NodeVisitor& visitor);
	virtual	void				Dump(DumpContext& context) const;

private:
			String				fRuleName;
			StringList			fParameterNames;
			Node*				fBlock;
};


void
RuleDefinition::AddParameterName(const String& parameterName)
{
	fParameterNames.Append(parameterName);
}


void
RuleDefinition::SetBlock(Node* block)
{
	fBlock = block;
}


}	// namespace code
}	// namespace ham


#endif	// HAM_CODE_RULE_DEFINITION_H
