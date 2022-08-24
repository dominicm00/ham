#ifndef HAM_TESTS_CODEUTILS_HPP
#define HAM_TESTS_CODEUTILS_HPP

#include "catch2/catch_message.hpp"
#include "code/Context.hpp"
#include "code/HamNodes.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"
#include "tao/pegtl/demangle.hpp"

#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace ham::tests
{

namespace p = tao::pegtl;

static std::string last_warning{};
static code::GlobalContext default_global_context{{"all"}, std::cerr, true};

code::AstContext
DefaultAstContext();

code::EvaluationContext
DefaultEvaluationContext();

class Nothing {};

struct TestNodeInfo {
	std::string content;
	std::string_view type;
	std::vector<TestNodeInfo> children;
};

template<class P>
TestNodeInfo
E(std::string str)
{
	return {.content = str, .type = p::demangle<P>()};
}

template<class P, class C = Nothing>
TestNodeInfo
N(std::vector<std::string> eval)
{
	// A node with str_node as children represents a test node that will
	// evaluate to the provided strings. Identity nodes hold their code class as
	// content, and parse class as type.
	TestNodeInfo node{};
	node.content = p::demangle<C>();
	node.type = p::demangle<P>();
	for (std::string& str : eval) {
		node.children.push_back(E<P>(str));
	}

	return std::move(node);
}

template<class P>
TestNodeInfo
W(std::vector<TestNodeInfo> nodes)
{
	TestNodeInfo node{};
	node.type = p::demangle<P>();
	node.children = std::move(nodes);
	return std::move(node);
}

std::unique_ptr<code::TestNode>
ConvertNodeInfo(TestNodeInfo node_info);

template<class P, class C>
requires std::derived_from<C, code::BasicNode<C>> data::List
Eval(
	code::EvaluationContext& test_eval_context,
	std::vector<TestNodeInfo> node_info
)
{
	TestNodeInfo eval_node{
		.content = std::string{p::demangle<C>()},
		.type = p::demangle<P>(),
		.children = std::move(node_info)};

	auto node = ConvertNodeInfo(eval_node);

	std::stringstream warn_stream{};
	code::GlobalContext global_context{{"all"}, warn_stream, true};
	code::AstContext ast_context{global_context};
	code::EvaluationContext eval_context{global_context};
	// port all eval data
	eval_context.variable_scope = test_eval_context.variable_scope;

	try {
		auto lst = C(ast_context, std::move(node)).Evaluate(eval_context);
		last_warning = warn_stream.str();
		UNSCOPED_INFO(last_warning);
		return std::move(lst);
	} catch (std::exception e) {
		last_warning = warn_stream.str();
		UNSCOPED_INFO(last_warning);
		throw;
	}
}

template<class P, class C>
requires std::derived_from<C, code::BasicNode<C>> data::List
Eval(std::vector<TestNodeInfo> nodes)
{
	auto eval_context = DefaultEvaluationContext();
	return Eval<P, C>(eval_context, std::move(nodes));
}

} // namespace ham::tests

#endif // HAM_TESTS_CODEUTILS_HPP
