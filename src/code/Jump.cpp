/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/Jump.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


namespace code {


template<typename JumpType>
Jump<JumpType>::Jump(Node* result)
	:
	fResult(result)
{
}


template<typename JumpType>
StringList
Jump<JumpType>::Evaluate(EvaluationContext& context)
{
	JumpType::Setup();
	return fResult->Evaluate(context);
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

#define HAM_DEFINE_JUMP_STATEMENT(name, statement)						\
	struct JumpType##name {												\
		static const char* const kName;									\
																		\
		static void Setup()												\
		{																\
			statement;													\
		}																\
	};																	\
																		\
	const char* const JumpType##name::kName = #name;					\
																		\
	template class Jump<JumpType##name>;


// TODO: Set correct jump statements!
HAM_DEFINE_JUMP_STATEMENT(Break,		(void)0)
HAM_DEFINE_JUMP_STATEMENT(Continue,		(void)0)
HAM_DEFINE_JUMP_STATEMENT(Return,		(void)0)
HAM_DEFINE_JUMP_STATEMENT(JumpToEof,	(void)0)

#undef HAM_DEFINE_JUMP_NODE


}	// namespace code
