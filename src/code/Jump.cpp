/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "code/Jump.hpp"

#include "code/DumpContext.hpp"
#include "code/EvaluationContext.hpp"

namespace ham::code
{

template<typename JumpType>
Jump<JumpType>::Jump(Node* result)
	: fResult(result)
{
	fResult->AcquireReference();
}

template<typename JumpType>
Jump<JumpType>::~Jump()
{
	fResult->ReleaseReference();
}

template<typename JumpType>
StringList
Jump<JumpType>::Evaluate(EvaluationContext& context)
{
	const auto result = fResult->Evaluate(context);
	JumpType::Setup(context);
	return result;
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

#define HAM_DEFINE_JUMP_STATEMENT(name, condition)           \
	struct JumpType##name {                                  \
		static const char* const kName;                      \
                                                             \
		static inline void Setup(EvaluationContext& context) \
		{                                                    \
			context.SetJumpCondition(condition);             \
		}                                                    \
	};                                                       \
                                                             \
	const char* const JumpType##name::kName = #name;         \
                                                             \
	template class Jump<JumpType##name>;

// TODO: Set correct jump statements!
HAM_DEFINE_JUMP_STATEMENT(Break, JUMP_CONDITION_BREAK)
HAM_DEFINE_JUMP_STATEMENT(Continue, JUMP_CONDITION_CONTINUE)
HAM_DEFINE_JUMP_STATEMENT(Return, JUMP_CONDITION_RETURN)
HAM_DEFINE_JUMP_STATEMENT(JumpToEof, JUMP_CONDITION_JUMP_TO_EOF)

#undef HAM_DEFINE_JUMP_NODE

} // namespace ham::code
