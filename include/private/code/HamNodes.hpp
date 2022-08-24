#ifndef HAM_CODE_BASICNODE_HPP
#define HAM_CODE_BASICNODE_HPP

#include "code/Context.hpp"
#include "code/Node.hpp"
#include "data/Types.hpp"
#include "parse/PegtlUtils.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"
#include "tao/pegtl/demangle.hpp"
#include "tao/pegtl/internal/frobnicator.hpp"
#include "util/HamError.hpp"

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
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

	virtual ~BasicNode<T>() = default;

	void Warning(GlobalContext& global_context, std::string_view message) const
	{
		HamWarning(global_context, pos, message);
	}

	[[nodiscard]] HamError Error(std::string_view message) const
	{
		return HamError(pos, message);
	}

	std::string_view Type() const override { return type; }

	// TODO: Redesign the class heiarchy to avoid this nonsense
	virtual data::List Evaluate(EvaluationContext&) const override
	{
		assert(false);
	};
	virtual std::string String() const override { assert(false); };
	virtual NodeDump Dump() const override { assert(false); };

  public:
	static constexpr std::string_view type = tao::pegtl::demangle<T>();

  private:
	Position pos;
	std::string_view content;
};

/**
 * Testing code
 */
class TestNode : public tao::pegtl::parse_tree::node {
  public:
	TestNode(std::string a_str_info)
		: str_info(std::move(a_str_info))
	{
		m_begin = tao::pegtl::internal::frobnicator(str_info.data());
		m_end = tao::pegtl::internal::frobnicator(
			str_info.data() + str_info.size()
		);
		source = std::string_view{str_info};
	};

	std::string str_info;
};

template<class T>
requires std::derived_from<T, BasicNode<T>>
class Identity : public Node {
  public:
	Identity<T>(PegtlNode&& pnode)
	{
		const TestNode* node = static_cast<const TestNode*>(pnode.get());
		if (node->str_info != tao::pegtl::demangle<T>()) {
			std::stringstream err;
			err << "tried to build " << tao::pegtl::demangle<T>() << " with "
				<< node->str_info;
			throw std::runtime_error(err.str());
		}

		// Use children as list elements
		for (const PegtlNode& child : node->children) {
			list.push_back(static_cast<const TestNode*>(child.get())->str_info);
		}
	};

	data::List Evaluate(EvaluationContext&) const { return list; }
	std::string String() const { return "Identity"; }
	std::string_view Type() const { return tao::pegtl::demangle<T>(); }
	NodeDump Dump() const { return {}; }

  private:
	data::List list;
};

template<class T>
requires std::derived_from<T, BasicNode<T>> std::unique_ptr<Node>
CreateNode(AstContext& ast_context, PegtlNode&& pegtl_node)
{
	if (ast_context.global_context.testing) {
		return std::make_unique<Identity<T>>(std::move(pegtl_node));
	} else {
		return std::make_unique<T>(ast_context, std::move(pegtl_node));
	}
}

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

} // namespace ham::code

#endif // HAM_CODE_BASICNODE_HPP
