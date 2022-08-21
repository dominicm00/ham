#ifndef HAM_CODE_NODE_HPP
#define HAM_CODE_NODE_HPP

#include "code/Context.hpp"
#include "data/Types.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace ham::code
{

class Node;

/**
 * Information dumped from node
 */
struct NodeDump {
	// type as determined by PEGTL demangle
	std::string_view type;
	// string content of this node if appropriate
	std::optional<std::string_view> content;
	// *all* nodes held interally by this node
	std::vector<std::reference_wrapper<const Node>> children;
};

/**
 * Nodes are the core type for all Ham operations. Ham files are parsed into a
 * tree of nodes that are then evaluated recursively.
 */
class Node {
  public:
	// The AST is final; nodes should not be copied.
	Node(const Node&) = delete;
	Node& operator=(const Node&) = delete;

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
	virtual data::List Evaluate(EvaluationContext&) const = 0;

	/**
	 * This method is used for testing. Dumps all node information.
	 */
	virtual NodeDump Dump() const = 0;

  public:
	/**
	 * A string representation of the class.
	 */
	static constexpr std::string_view type;
};

} // namespace ham::code

#endif // HAM_CODE_NODE_HPP
