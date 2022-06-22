/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_INCLUDE_HPP
#define HAM_CODE_INCLUDE_HPP

#include "code/Node.hpp"

namespace ham
{
namespace code
{

class Include : public Node
{
  public:
	Include(Node* fileNames);
	virtual ~Include();

	virtual StringList Evaluate(EvaluationContext& context);
	virtual Node* Visit(NodeVisitor& visitor);
	virtual void Dump(DumpContext& context) const;

  private:
	Node* fFileNames;
};

} // namespace code
} // namespace ham

#endif // HAM_CODE_INCLUDE_HPP
