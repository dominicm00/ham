/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_ACTIONS_DEFINITION_H
#define HAM_CODE_ACTIONS_DEFINITION_H


#include <stdint.h>

#include "code/Node.h"


namespace code {


class ActionsDefinition : public Node {
public:
								ActionsDefinition(uint32_t flags,
									const String& identifier, Node* variables,
									const String& actions);

	virtual	StringList			Evaluate(EvaluationContext& context);

	virtual	void				Dump(DumpContext& context) const;

private:
			String				fIdentifier;
			Node*				fVariables;
			String				fActions;
			uint32_t			fFlags;
};


}	// namespace code


#endif	// HAM_CODE_ACTIONS_DEFINITION_H
