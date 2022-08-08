#include "tests/Utils.hpp"

#include "catch2/catch_message.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"

#include <memory>
#include <stdexcept>
#include <utility>

namespace ham::tests
{

namespace p = tao::pegtl;
using NodePointer = std::unique_ptr<p::parse_tree::node>;

NodePointer
decompose(NodePointer&& node, std::vector<int> indices)
{
	if (!node)
		return NodePointer{};

	for (int i = 0; i < indices.size(); i++) {
		if (!node || node->children.empty())
			return NodePointer{};
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

bool
checkParse(NodePointer&& node, NodeStructure ns, std::size_t depth)
{
	if (!node) {
		UNSCOPED_INFO("Node is empty.");
		return false;
	}
	std::string indent{"  ", depth};
	std::string nodeType = strip(node->type);
	std::string nodeString = node->has_content() ? node->string() : "";
	std::stringstream infoMessage{};
	bool isValid = true;

	infoMessage << indent << nodeType << '[' << node->children.size() << ']'
				<< '{' << nodeString << '}';
	if (nodeType != ns.type
		|| (!ns.content.empty() && (nodeString != ns.content))
		|| (!ns.leaf && node->children.size() != ns.children.size())) {
		infoMessage << " != " << ns.type << '[';

		if (ns.leaf)
			infoMessage << "?";
		else
			infoMessage << ns.children.size();

		infoMessage << ']' << '{' << ns.content << '}';
		isValid = false;
	}
	UNSCOPED_INFO(infoMessage.str());

	if (ns.leaf)
		return isValid;

	for (int i = 0; i < std::min(node->children.size(), ns.children.size());
		 i++) {
		isValid = checkParse(
					  std::forward<NodePointer>(node->children[i]),
					  ns.children[i],
					  depth + 1
				  )
			&& isValid;
	}

	return isValid;
}

bool
checkParse(NodePointer&& node, NodeStructure ns)
{
	return checkParse(std::forward<NodePointer>(node), ns, 0);
}

} // namespace ham::tests
