#ifndef HAM_CODE_BASICNODE_HPP
#define HAM_CODE_BASICNODE_HPP

#include "code/Context.hpp"
#include "code/Node.hpp"
#include "parse/PegtlUtils.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"
#include "tao/pegtl/demangle.hpp"
#include "util/HamError.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <string_view>
#include <variant>

namespace ham::code
{

using PegtlNode = std::unique_ptr<tao::pegtl::parse_tree::node>;

/**
 * Constructs functionality for a basic Ham node, complying with the public Node
 * API.
 */
template<typename T>
class BasicNode : public Node {
  public:
	BasicNode<T>() = delete;
	BasicNode<T>(AstContext& ast_context, PegtlNode& pegtl_node)
		: pos(
			parse::ConvertToHamPosition(pegtl_node->begin(), pegtl_node->end())
		){};

	void Warning(GlobalContext& global_context, std::string_view message) const
	{
		HamWarning(global_context, pos, message);
	}

	[[nodiscard]] HamError Error(std::string_view message) const
	{
		return HamError(pos, message);
	}

	virtual data::List Evaluate(EvaluationContext&) const override = 0;
	virtual NodeDump Dump() const override = 0;

  public:
	static constexpr std::string_view type = tao::pegtl::demangle<T>();

  private:
	Position pos;
	std::string_view content;
};

/**
 * Ham nodes
 */
class Variable;
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

/**
 * Node interfaces are (rather verbosely) all stored in this file because
 * they are _very_ recursive, and trying to manage imports/forward
 * declarations across header files in a way that doesn't care about import
 * order and works well with editors is annoying.
 */

// Helper type for visiting variants
template<class... Ts>
struct overloaded : Ts... {
	using Ts::operator()...;
};
// Explicit deduction guide
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

/**
 * Identifier
 */
class Identifier : public BasicNode<Identifier> {
	using IdPart = std::variant<std::string, std::unique_ptr<Variable>>;

  public:
	Identifier(AstContext& ast_context, PegtlNode&& pegtl_node);

	data::List Evaluate(EvaluationContext& eval_context) const;
	NodeDump Dump() const;
	friend std::ostream& operator<<(std::ostream&, const Identifier&);

  private:
	std::string content;
	std::vector<IdPart> id_parts;
};

class Variable : public BasicNode<Variable> {
  public:
	Variable(AstContext& ast_context, PegtlNode&& pegtl_node);

	data::List Evaluate(EvaluationContext& eval_context) const;
	NodeDump Dump() const;
	friend std::ostream& operator<<(std::ostream&, const Variable&);

  private:
	std::string content;
	std::unique_ptr<Identifier> id;
	std::unique_ptr<Identifier> subscript;
};

} // namespace ham::code

#endif // HAM_CODE_BASICNODE_HPP
