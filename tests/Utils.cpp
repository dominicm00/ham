#include "tests/Utils.hpp"

#include "tao/pegtl/contrib/parse_tree.hpp"

#include <memory>
#include <stdexcept>

namespace ham::tests
{

namespace p = tao::pegtl;

std::unique_ptr<p::parse_tree::node>
decompose(std::unique_ptr<p::parse_tree::node>&& node, std::vector<int> indices)
{
	for (int i = 0; i < indices.size(); i++) {
		if (node->children.empty())
			return std::unique_ptr<p::parse_tree::node>{};
		node = std::move(node->children[indices[i]]);
	}
	return node;
};

std::string
strip(std::string_view type)
{
	const auto pos = type.find_last_of(':');
	if (pos == std::string::npos) {
		return std::string{type};
	} else {
		return std::string{type.substr(pos + 1)};
	}
}

void
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

void
check(std::unique_ptr<p::parse_tree::node>& node, NodeStructure ns)
{
	check(node, ns, 0);
}

} // namespace ham::tests
