#ifndef HAM_CODE_BASICNODE_HPP
#define HAM_CODE_BASICNODE_HPP

#include "code/Node.hpp"
#include "tao/pegtl/demangle.hpp"

#include <functional>
#include <optional>
#include <string_view>

namespace ham::code
{

/**
 * Constructs functionality for a basic Ham node, complying with the public Node
 * API.
 */
template<typename T>
class BasicNode : public Node {
  public:
	static constexpr std::string_view type = tao::pegtl::demangle<T>();
};

/**
 * Implement visit/dump for leaf nodes
 */
template<typename T>
class LeafNode : public BasicNode<T> {
  public:
	std::optional<std::reference_wrapper<const Node>> Visit(NodeVisitor& visitor
	) const
	{
		if (visitor.VisitNode(*this))
			return *this;
		else
			return {};
	}

	NodeDump Dump()
	{
		return NodeDump{
			.class_name = this->type,
			.content = this->content,
			.children{}};
	}

  private:
	std::string_view content;
};

/**
 * Ham nodes
 */
class Identifier;
class Leaf;
class List;

class ForLoop;
class IfStatement;
class WhileLoop;

class ActionDefinition;
class RuleActionInvocation;
class RuleDefinition;
class StatementBlock;
class VariableAssignment;

class BooleanExpression;
class LeafComparator;
class TargetExpression;
class VariableExpression;

} // namespace ham::code

#endif // HAM_CODE_BASICNODE_HPP
