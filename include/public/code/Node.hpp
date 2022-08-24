#ifndef HAM_CODE_NODE_HPP
#define HAM_CODE_NODE_HPP

#include "code/Context.hpp"
#include "data/Types.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <ostream>
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
	Node() = default;
	virtual ~Node() = default;

	// Nodes are managed by unique_ptr and cannot be moved/copied
	Node(Node&) = delete;
	Node& operator=(const Node&) = delete;
	Node(Node&&) = delete;
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
	virtual data::List Evaluate(EvaluationContext&) const = 0;

	/**
	 * String representation of the node type
	 */
	[[nodiscard]] virtual std::string_view Type() const = 0;

	/**
	 * String representation of current node (error/warning output)
	 */
	[[nodiscard]] virtual std::string String() const = 0;

	/**
	 * This method is used for testing. Dumps all node information.
	 */
	[[nodiscard]] virtual NodeDump Dump() const = 0;
};

} // namespace ham::code

#endif // HAM_CODE_NODE_HPP
