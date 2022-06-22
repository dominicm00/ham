/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_ACTIONS_DEFINITION_HPP
#define HAM_CODE_ACTIONS_DEFINITION_HPP

#include <stdint.h>

#include "code/Node.hpp"

namespace ham::code
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

} // namespace ham::code

#endif // HAM_CODE_ACTIONS_DEFINITION_HPP
