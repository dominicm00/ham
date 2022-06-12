/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_ACTIONS_DEFINITION_H
#define HAM_CODE_ACTIONS_DEFINITION_H

#include <stdint.h>

#include "code/Node.h"

namespace ham
{
namespace code
{

class ActionsDefinition : public Node
{
  public:
	ActionsDefinition(uint32_t flags,
					  const String& ruleName,
					  Node* variables,
					  const String& actions);
	virtual ~ActionsDefinition();

	virtual StringList Evaluate(EvaluationContext& context);
	virtual Node* Visit(NodeVisitor& visitor);
	virtual void Dump(DumpContext& context) const;

  private:
	String fRuleName;
	Node* fVariables;
	String fActions;
	uint32_t fFlags;
};

} // namespace code
} // namespace ham

#endif // HAM_CODE_ACTIONS_DEFINITION_H
