/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_CASE_HPP
#define HAM_CODE_CASE_HPP

#include "code/Node.hpp"

namespace ham::code
{

class Case : public Node
{
  public:
	Case(const String& pattern, Node* block);
	virtual ~Case();

	bool Matches(EvaluationContext& context, const StringList& value) const;

	virtual StringList Evaluate(EvaluationContext& context);
	virtual Node* Visit(NodeVisitor& visitor);
	virtual void Dump(DumpContext& context) const;

  private:
	String fPattern;
	Node* fBlock;
};

} // namespace ham::code

#endif // HAM_CODE_CASE_HPP
