#ifndef HAM_TESTS_UTILS_HPP
#define HAM_TESTS_UTILS_HPP

#include "catch2/catch_test_macros.hpp"
#include "parse/Grammar.hpp"
#include "parse/NodeParser.hpp"
#include "tao/pegtl.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"
#include "tao/pegtl/demangle.hpp"
#include "tao/pegtl/internal/pegtl_string.hpp"
#include "tao/pegtl/memory_input.hpp"
#include "tao/pegtl/rules.hpp"
#include "tao/pegtl/string_input.hpp"
#include "tao/pegtl/tracking_mode.hpp"

#include <memory>

namespace ham::tests
{

struct NodeStructure {
	NodeStructure(
		std::string type,
		std::string content = {},
		std::vector<NodeStructure> children = {},
		bool leaf = false
	)
		: type(type),
		  content(content),
		  children(children),
		  leaf(leaf){};

	std::string type;
	std::string content;
	std::vector<NodeStructure> children;
	bool leaf;
};

template<typename Rule>
std::unique_ptr<p::parse_tree::node>
genericParse(const std::string& str)
{
	return parse::NodeParser::ParseNode<p::sor<
		p::seq<Rule, p::eof>,
		TAO_PEGTL_RAISE_MESSAGE("failed to match whole string")>>(
		p::memory_input<p::tracking_mode::eager>{str, "tests"}
	);
};

std::unique_ptr<p::parse_tree::node>
decompose(
	std::unique_ptr<p::parse_tree::node>&& node,
	std::vector<int> indices
);

std::string
strip(std::string_view type);

bool
checkParse(
	std::unique_ptr<p::parse_tree::node>&& node,
	NodeStructure ns,
	std::size_t depth
);

bool
checkParse(std::unique_ptr<p::parse_tree::node>&& node, NodeStructure ns);

template<typename Type>
NodeStructure
T(std::string content = {})
{
	return NodeStructure(strip(p::demangle<Type>()), content, {}, true);
}

template<typename Type>
NodeStructure
T(std::string content, std::vector<NodeStructure> children)
{
	return NodeStructure(strip(p::demangle<Type>()), content, children, false);
}

template<typename Type>
NodeStructure
T(std::string content, bool leaf)
{
	return NodeStructure(strip(p::demangle<Type>()), content, {}, leaf);
}

template<typename Type>
NodeStructure
T(std::vector<NodeStructure> children)
{
	return NodeStructure(strip(p::demangle<Type>()), "", children, false);
}

} // namespace ham::tests

#define REQUIRE_NODE(node, ns) REQUIRE(checkParse(node, ns))

#define REQUIRE_PARSE(str, ns) REQUIRE_NODE(decompose(parse(str), {0}), ns)

#endif // HAM_TESTS_UTILS_HPP
