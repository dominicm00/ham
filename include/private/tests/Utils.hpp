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

namespace ham::tests
{

namespace p = tao::pegtl;

template<typename Rule>
inline auto
genericParse(const std::string& str)
{
	auto input = p::memory_input{str, "tests"};
	return p::parse_tree::parse<p::seq<Rule, p::eof>, ham::code::Selector>(input
	);
};

inline auto
decompose(std::unique_ptr<p::parse_tree::node>&& node, std::vector<int> indices)
{
	for (int i = 0; i < indices.size(); i++) {
		if (node->children.empty())
			return std::unique_ptr<p::parse_tree::node>{};
		node = std::move(node->children[indices[i]]);
	}
	return node;
};

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

inline std::string
strip(std::string_view type)
{
	const auto pos = type.find_last_of(':');
	if (pos == std::string::npos) {
		return std::string{type};
	} else {
		return std::string{type.substr(pos + 1)};
	}
}

inline void
check(
	std::unique_ptr<p::parse_tree::node>& node,
	NodeStructure ns,
	std::size_t depth
)
{
	std::string indent{"  ", depth};
	std::string nodeType = strip(node->type);
	std::string nodeString = node->has_content() ? node->string() : "";
	std::stringstream infoMessage{};

	infoMessage << indent << nodeType << '[' << nodeString << ']';
	if (nodeType != ns.type
		|| (!ns.content.empty() && (nodeString != ns.content))) {
		infoMessage << " != " << ns.type << '[' << ns.content << ']';
	}
	INFO(infoMessage.str());

	REQUIRE(nodeType == ns.type);
	if (!ns.content.empty()) {
		REQUIRE(nodeString == ns.content);
	}
	REQUIRE(node->children.size() == ns.children.size());
	for (int i = 0; i < ns.children.size(); i++) {
		check(node->children[i], ns.children[i], depth + 1);
	}
}

inline void
check(std::unique_ptr<p::parse_tree::node>& node, NodeStructure ns)
{
	check(node, ns, 0);
}

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

#endif // HAM_TESTS_UTILS_HPP
