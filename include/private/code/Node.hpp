#ifndef HAM_CODE_NODE_HPP
#define HAM_CODE_NODE_HPP

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace ham::code
{

class Node;

/**
 * Passed to Node::Visit to recursively visit nodes.
 */
class NodeVisitor {
  public:
	virtual ~NodeVisitor();

	/**
	 * Called on all Node objects in a subtree.
	 *
	 * \param[in] node Node to visit.
	 * \return whether or not to stop visiting nodes.
	 */
	virtual bool VisitNode(Node& node) = 0;
};

/**
 * Context for building the AST.
 */
class AstContext {
  public:
	void EnterLoop()
	{
		loop_depth++;
		block_depth++;
	}

	void EnterBlock() { block_depth++; }

  public:
	unsigned int loop_depth;
	unsigned int block_depth;
};

/**
 * Context for evaluating AST
 */
class EvaluationContext {};

/**
 * Information dumped from node
 */
class NodeDump {
  public:
	// class name as determined by PEGTL demangle
	std::string class_name;
	// string content of this node if appropriate
	std::optional<std::string> content;
	// *all* nodes held interally by this node
	std::vector<std::reference_wrapper<const Node>> children;
};

/**
 * Nodes are the core type for all Ham operations. Ham files are parsed into a
 * tree of nodes that are then evaluated recursively.
 */
class Node {
  public:
	// Nodes should only be constructed via subclasses.
	Node() = delete;
	// The AST is final; nodes should not be copied or moved.
	Node(const Node&) = delete;
	Node(const Node&&) = delete;
	Node& operator=(const Node&) = delete;
	Node& operator=(const Node&&) = delete;

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
	virtual std::vector<std::string> Evaluate(EvaluationContext& context) = 0;

	/**
	 * Visit nodes recursively until NodeVisitor::VisitNode returns true. Nodes
	 * are recursed in left-to-right order.
	 *
	 * Nodes should implement this by first checking if
	 * `visitor.VisitNode(this)` returns true, and returning `this` if it
	 * does. Otherwise, they should check each subnode and return the first
	 * match. A simple example is provided below:

	 \code
	 if (visitor.VisitNode(this))
	   return this;

	 if (Node& result = fLeft.Visit(visitor))
	   return result;

	 if (Node& result = fRight.Visit(visitor))
	   return result;
	 \endcode

	 * \param[in] visitor Contains NodeVisitor::VisitNode predicate.
	 *
	 * \result First Node to match predicate.
	 */
	virtual std::optional<std::reference_wrapper<Node>> Visit(
		NodeVisitor& visitor
	) const = 0;

  private:
	/**
	 * This method is used for testing. Dumps all node information.
	 */
	virtual NodeDump Dump() const = 0;
};
} // namespace ham::code

#endif // HAM_CODE_NODE_HPP
