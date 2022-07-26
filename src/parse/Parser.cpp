#include "parse/Parser.hpp"

#include "parse/Grammar.hpp"
#include "tao/pegtl.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"
#include "tao/pegtl/contrib/parse_tree_to_dot.hpp"
#include "tao/pegtl/string_input.hpp"

#include <iostream>
#include <string>

namespace ham::parse
{

void
Parser::Parse(std::string str)
{
	auto in = p::string_input{str, "main"};
	const auto root =
		p::parse_tree::parse<Grammar::Statements, Grammar::Selector>(in);
	if (root) {
		p::parse_tree::print_dot(std::cout, *root);
	}
	return;
}

} // namespace ham::parse
