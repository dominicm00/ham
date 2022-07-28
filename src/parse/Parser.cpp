#include "parse/Parser.hpp"

#include "parse/Grammar.hpp"
#include "tao/pegtl.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"
#include "tao/pegtl/contrib/parse_tree_to_dot.hpp"
#include "tao/pegtl/contrib/print.hpp"
#include "tao/pegtl/string_input.hpp"

#include <iostream>
#include <string>

namespace ham::parse
{

void
Parser::Parse(std::string str)
{
	auto in = p::string_input{str, "main"};
#if 0
	p::print_debug<parse::statements>(std::cout);
#endif

#if 1
	const auto root =
		p::parse_tree::parse<parse::statements, parse::selector>(in);
	if (root) {
		p::parse_tree::print_dot(std::cout, *root);
	}
#endif

	return;
}

} // namespace ham::parse
