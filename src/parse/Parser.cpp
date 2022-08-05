#include "parse/Parser.hpp"

#include "parse/Grammar.hpp"
#include "tao/pegtl.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"
#include "tao/pegtl/contrib/parse_tree_to_dot.hpp"
#include "tao/pegtl/contrib/print.hpp"
#include "tao/pegtl/contrib/trace.hpp"
#include "tao/pegtl/rules.hpp"
#include "tao/pegtl/string_input.hpp"

#include <iostream>
#include <string>

namespace ham::parse
{

void
Parser::Parse(std::string str, Debug debug)
{
	if (debug == GRAMMAR) {
		p::print_debug<parse::statements>(std::cout);
		return;
	}

	auto in = p::string_input{str, "main"};
	const auto parse = [&in]()
	{
		return p::parse_tree::
			parse<p::must<parse::statements>, parse::selector>(in);
	};

	switch (debug) {
		case GRAMMAR:
			return;
		case NONE:
			break;
		case XDOT:
			p::parse_tree::print_dot(std::cout, *parse());
			break;
		case TRACE:
			p::standard_trace<p::must<parse::statements>>(in);
			break;
	}

	return;
}

} // namespace ham::parse
