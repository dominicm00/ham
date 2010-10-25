/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_ACTIONS_DEFINITION_H
#define HAM_CODE_ACTIONS_DEFINITION_H


#include <stdint.h>

#include "code/Node.h"


namespace code {


static const uint32_t kActionFlagUpdated		= 0x01;
static const uint32_t kActionFlagTogether		= 0x02;
static const uint32_t kActionFlagIgnore			= 0x04;
static const uint32_t kActionFlagQuietly		= 0x08;
static const uint32_t kActionFlagPiecemeal		= 0x10;
static const uint32_t kActionFlagExisting		= 0x20;
static const uint32_t kActionFlagMask			= 0x3f;
static const uint32_t kActionFlagMaxLineFactor	= 0x40;


class ActionsDefinition : public Node {
public:
								ActionsDefinition(uint32_t flags,
									const String& identifier, Node* variables,
									const String& actions);
	virtual						~ActionsDefinition();

	virtual	StringList			Evaluate(EvaluationContext& context);
	virtual	Node*				Visit(NodeVisitor& visitor);
	virtual	void				Dump(DumpContext& context) const;

private:
			String				fIdentifier;
			Node*				fVariables;
			String				fActions;
			uint32_t			fFlags;
};


}	// namespace code


#endif	// HAM_CODE_ACTIONS_DEFINITION_H
