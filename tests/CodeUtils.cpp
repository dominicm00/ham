#include "tests/CodeUtils.hpp"

namespace ham::tests
{

code::AstContext
DefaultAstContext()
{
	return code::AstContext(default_global_context);
}

code::EvaluationContext
DefaultEvaluationContext()
{
	return code::EvaluationContext(default_global_context);
}

std::unique_ptr<code::TestNode>
ConvertNodeInfo(TestNodeInfo node_info)
{
	std::unique_ptr<code::TestNode> node{
		std::make_unique<code::TestNode>(node_info.content)};
	node->type = node_info.type;
	for (auto& child : node_info.children) {
		node->children.push_back(ConvertNodeInfo(child));
	}
	return std::move(node);
}

} // namespace ham::tests
