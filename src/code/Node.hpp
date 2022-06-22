/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_CODE_HPP
#define HAM_CODE_CODE_HPP

#include <list>

#include "data/StringList.hpp"
#include "util/Referenceable.hpp"

namespace ham::code
{

class DumpContext;
class EvaluationContext;
class Node;

using util::Reference;
using util::Referenceable;

/**
 * Passed to Node::Visit to recursively visit nodes.
 */
class NodeVisitor
{
  public:
	virtual ~NodeVisitor();

    /**
	 * Called on all Node objects in a subtree.
	 *
	 * \param[in] node Node to visit.
	 * \return whether or not to stop visiting nodes.
	 */
	virtual bool VisitNode(Node* node) = 0;
};

/**
 * Nodes are the core type for all Ham operations. Ham files are parsed into a
 * tree of nodes that are then evaluated recursively.
 */
class Node : public Referenceable
{
  public:
	virtual ~Node();

	/**
	 * Evaluate the current node. This generally involves evaluating any
	 * subnodes, then performing the operation the current node represents.
	 *
	 * \param[in] context Context to evaluate node under.
	 *
	 * \result If the current node is an expression, return the StringList
	 * associated with the expression result, otherwise return
	 * StringList::kFalse.
	 */
	virtual StringList Evaluate(EvaluationContext& context) = 0;

	/**
	 * Visit nodes recursively until predicate is met.
	 *
	 * Nodes should implement this by first checking if
	 * `visitor.VisitNode(this)` returns true, and returning `this` if it
	 * does. Otherwise, they should check each subnode and return the first
	 * match. A simple example is provided below:

	 \code
     if (visitor.VisitNode(this))
	   return this;

	 if (Node* result = fLeft->Visit(visitor))
	   return result;

	 if (Node* result = fRight->Visit(visitor))
	   return result;
	 \endcode

	 * \param[in] visitor Contains NodeVisitor::VisitNode predicate.
	 *
	 * \result First Node to match predicate.
	 */
	virtual Node* Visit(NodeVisitor& visitor) = 0;

	/**
	 * Dump a string representation of the current node into
	 * code::DumpContext. Subnodes should be dumped recursively, wrapped in
	 * calls to DumpContext::BeginChildren and DumpContext::EndChildren. A
	 * simple example is provided below:

	 \code
	 context << "BinaryExpressionNode(\n";
	 context.BeginChildren();

	 fLeft->Dump(context);
	 fRight->Dump(context);

	 context.EndChildren();
	 context << ")\n";
	 \endcode

	 * \param[in] context Dump context for outputting string and denoting
	 * children.
	 */
	virtual void Dump(DumpContext& context) const = 0;
};

typedef std::list<Node*> NodeList;
typedef Reference<Node> NodeReference;

} // namespace ham::code

#endif // HAM_CODE_CODE_HPP
