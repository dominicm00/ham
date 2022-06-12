/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_RULE_DEFINITION_H
#define HAM_CODE_RULE_DEFINITION_H

#include "code/Node.h"

namespace ham
{
namespace code
{

class Block;
class RuleInstructions;

class RuleDefinition : public Node
{
  public:
	RuleDefinition(const String& ruleName,
				   const StringList& parameterNames,
				   Block* block);
	virtual ~RuleDefinition();

	virtual StringList Evaluate(EvaluationContext& context);
	virtual Node* Visit(NodeVisitor& visitor);
	virtual void Dump(DumpContext& context) const;

  private:
	String fRuleName;
	StringList fParameterNames;
	Block* fBlock;
	RuleInstructions* fInstructions;
};

} // namespace code
} // namespace ham

#endif // HAM_CODE_RULE_DEFINITION_H
