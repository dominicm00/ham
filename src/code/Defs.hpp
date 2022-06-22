/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_DEFS_HPP
#define HAM_CODE_DEFS_HPP

namespace ham
{
namespace code
{

enum AssignmentOperator {
	ASSIGNMENT_OPERATOR_ASSIGN,
	ASSIGNMENT_OPERATOR_APPEND,
	ASSIGNMENT_OPERATOR_DEFAULT
};

enum JumpCondition {
	JUMP_CONDITION_NONE,
	JUMP_CONDITION_BREAK,
	JUMP_CONDITION_CONTINUE,
	JUMP_CONDITION_RETURN,
	JUMP_CONDITION_JUMP_TO_EOF,
	JUMP_CONDITION_EXIT
};

} // namespace code
} // namespace ham

#endif // HAM_CODE_DEFS_HPP
