/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_LIST_H
#define HAM_CODE_LIST_H

#include <vector>

#include "code/Node.h"

namespace ham
{
namespace code
{

class List : public Node
{
  public:
	List();
	virtual ~List();

	inline void AppendKeepReference(Node* child);

	virtual StringList Evaluate(EvaluationContext& context);
	virtual Node* Visit(NodeVisitor& visitor);
	virtual void Dump(DumpContext& context) const;

  private:
	std::vector<Node*> fChildren;
};

void
List::AppendKeepReference(Node* child)
{
	try {
		fChildren.push_back(child);
	} catch (...) {
		child->ReleaseReference();
	}
}

} // namespace code
} // namespace ham

#endif // HAM_CODE_LIST_H
