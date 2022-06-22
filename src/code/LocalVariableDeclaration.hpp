/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_LOCAL_VARIABLE_DECLARATION_HPP
#define HAM_CODE_LOCAL_VARIABLE_DECLARATION_HPP

#include "code/Node.hpp"

namespace ham::code
{

class LocalVariableDeclaration : public Node
{
  public:
	LocalVariableDeclaration(Node* variables, Node* initializer);
	virtual ~LocalVariableDeclaration();

	virtual StringList Evaluate(EvaluationContext& context);
	virtual Node* Visit(NodeVisitor& visitor);
	virtual void Dump(DumpContext& context) const;

  private:
	Node* fVariables;
	Node* fInitializer;
};

} // namespace ham::code

#endif // HAM_CODE_LOCAL_VARIABLE_DECLARATION_HPP
