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

template<typename Rule>
bool
genericIdentity(const std::string& str)
{
	std::unique_ptr<p::parse_tree::node> node = genericParse<Rule>(str);
	return node && node->has_content() && node->string_view() == str;
}

template<typename Rule>
std::string
genericContent(const std::string& str)
{
	std::unique_ptr<p::parse_tree::node> node = genericParse<Rule>(str);
	if (!node)
		throw std::runtime_error("Failed to parse node.");
	if (!node->has_content())
		return "";
	return node->string();
}

std::unique_ptr<p::parse_tree::node>
decompose(
	std::unique_ptr<p::parse_tree::node>&& node,
	std::vector<int> indices
);

std::string
strip(std::string_view type);

void
check(
	std::unique_ptr<p::parse_tree::node>& node,
	NodeStructure ns,
	std::size_t depth
);

void
check(std::unique_ptr<p::parse_tree::node>& node, NodeStructure ns);

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

#define PARSE_FUNCTIONS(rule)                 \
	const auto parse = genericParse<rule>;    \
	const auto identity = genericParse<rule>; \
	const auto content = genericContent<rule>;

#endif // HAM_TESTS_UTILS_HPP
