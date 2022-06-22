/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_CONSTANT_HPP
#define HAM_CODE_CONSTANT_HPP

#include "code/Node.hpp"

namespace ham
{
namespace code
{

class Constant : public Node
{
  public:
	Constant(const StringList& value);
	virtual ~Constant();

	virtual StringList Evaluate(EvaluationContext& context);
	virtual Node* Visit(NodeVisitor& visitor);
	virtual void Dump(DumpContext& context) const;

  private:
	StringList fValue;
};

} // namespace code
} // namespace ham

#endif // HAM_CODE_CONSTANT_HPP
