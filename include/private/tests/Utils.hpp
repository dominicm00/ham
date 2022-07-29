#ifndef HAM_TESTS_UTILS_HPP
#define HAM_TESTS_UTILS_HPP

#include "catch2/catch_test_macros.hpp"
#include "parse/Grammar.hpp"
#include "tao/pegtl.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"
#include "tao/pegtl/demangle.hpp"
#include "tao/pegtl/memory_input.hpp"
#include "tao/pegtl/rules.hpp"
#include "tao/pegtl/string_input.hpp"

#include <memory>

namespace ham::tests
{

struct NodeStructure {
	NodeStructure(
		std::string type,
		std::string content = {},
		std::vector<NodeStructure> children = {}
	)
		: type(type),
		  content(content),
		  children(children){};

	std::string type;
	std::string content;
	std::vector<NodeStructure> children;
};

template<typename Rule>
std::unique_ptr<p::parse_tree::node>
genericParse(const std::string& str)
{
	auto input = p::memory_input{str, "tests"};
	return p::parse_tree::parse<p::seq<Rule, p::eof>, ham::parse::selector>(
		input
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
T(std::string content = {}, std::vector<NodeStructure> children = {})
{
	return NodeStructure(strip(p::demangle<Type>()), content, children);
}

template<typename Type>
NodeStructure
T(std::vector<NodeStructure> children)
{
	return NodeStructure(strip(p::demangle<Type>()), "", children);
}

} // namespace ham::tests

#define REQUIRE_NODE(node, ns) REQUIRE(checkParse(node, ns))

#define REQUIRE_PARSE(str, ns) REQUIRE_NODE(decompose(parse(str), {0}), ns)

#endif // HAM_TESTS_UTILS_HPP
