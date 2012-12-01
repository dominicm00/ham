/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/Jump.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


namespace ham {
namespace code {


template<typename JumpType>
Jump<JumpType>::Jump(Node* result)
	:
	fResult(result)
{
}


template<typename JumpType>
Jump<JumpType>::~Jump()
{
	delete fResult;
}


template<typename JumpType>
StringList
Jump<JumpType>::Evaluate(EvaluationContext& context)
{
	JumpType::Setup(context);
	return fResult->Evaluate(context);
}


template<typename JumpType>
code::Node*
Jump<JumpType>::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	return fResult->Visit(visitor);
}


template<typename JumpType>
void
Jump<JumpType>::Dump(DumpContext& context) const
{
	context << "Jump[" << JumpType::kName << "](\n";
	context.BeginChildren();

	fResult->Dump(context);

	context.EndChildren();
	context << ")\n";
}


// define and instantiate the specializations

#define HAM_DEFINE_JUMP_STATEMENT(name, condition)						\
	struct JumpType##name {												\
		static const char* const kName;									\
																		\
		static inline void Setup(EvaluationContext& context)			\
		{																\
			context.SetJumpCondition(condition);						\
		}																\
	};																	\
																		\
	const char* const JumpType##name::kName = #name;					\
																		\
	template class Jump<JumpType##name>;


// TODO: Set correct jump statements!
HAM_DEFINE_JUMP_STATEMENT(Break,		JUMP_CONDITION_BREAK)
HAM_DEFINE_JUMP_STATEMENT(Continue,		JUMP_CONDITION_CONTINUE)
HAM_DEFINE_JUMP_STATEMENT(Return,		JUMP_CONDITION_RETURN)
HAM_DEFINE_JUMP_STATEMENT(JumpToEof,	JUMP_CONDITION_JUMP_TO_EOF)

#undef HAM_DEFINE_JUMP_NODE


}	// namespace code
}	// namespace ham
