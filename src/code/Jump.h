/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_JUMP_H
#define HAM_CODE_JUMP_H


#include "code/Node.h"


namespace code {


template<typename JumpType>
class Jump : public Node {
public:
								Jump(Node* result);

	virtual	StringList			Evaluate(EvaluationContext& context);

	virtual	void				Dump(DumpContext& context) const;

private:
			Node*				fResult;
};


// declare the specializations

#define HAM_DECLARE_JUMP_STATEMENT(name)				\
	struct JumpType##name;								\
	typedef Jump<JumpType##name> name;

HAM_DECLARE_JUMP_STATEMENT(Break)
HAM_DECLARE_JUMP_STATEMENT(Continue)
HAM_DECLARE_JUMP_STATEMENT(Return)
HAM_DECLARE_JUMP_STATEMENT(JumpToEof)

#undef HAM_DECLARE_JUMP_NODE


}	// namespace code


#endif	// HAM_CODE_JUMP_H
