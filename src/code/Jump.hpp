/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_JUMP_HPP
#define HAM_CODE_JUMP_HPP

#include "code/Node.hpp"

namespace ham
{
namespace code
{

template<typename JumpType>
class Jump : public Node
{
  public:
	Jump(Node* result);
	virtual ~Jump();

	virtual StringList Evaluate(EvaluationContext& context);
	virtual Node* Visit(NodeVisitor& visitor);
	virtual void Dump(DumpContext& context) const;

  private:
	Node* fResult;
};

// declare the specializations

#define HAM_DECLARE_JUMP_STATEMENT(name)                                       \
	struct JumpType##name;                                                     \
	typedef Jump<JumpType##name> name;

HAM_DECLARE_JUMP_STATEMENT(Break)
HAM_DECLARE_JUMP_STATEMENT(Continue)
HAM_DECLARE_JUMP_STATEMENT(Return)
HAM_DECLARE_JUMP_STATEMENT(JumpToEof)

#undef HAM_DECLARE_JUMP_STATEMENT

} // namespace code
} // namespace ham

#endif // HAM_CODE_JUMP_HPP
