/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_FUNCTION_CALL_HPP
#define HAM_CODE_FUNCTION_CALL_HPP

#include "code/Node.hpp"

namespace ham::code
{

class FunctionCall : public Node
{
  public:
	FunctionCall(Node* function);
	FunctionCall(Node* function, const NodeList& arguments);
	virtual ~FunctionCall();

	inline void AddArgument(Node* argument);

	virtual StringList Evaluate(EvaluationContext& context);
	virtual Node* Visit(NodeVisitor& visitor);
	virtual void Dump(DumpContext& context) const;

  private:
	typedef NodeList ArgumentList;

	Node* fFunction;
	ArgumentList fArguments;
};

void
FunctionCall::AddArgument(Node* argument)
{
	fArguments.push_back(argument);
	argument->AcquireReference();
}

} // namespace ham::code

#endif // HAM_CODE_FUNCTION_CALL_HPP
