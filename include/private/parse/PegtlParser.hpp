#ifndef HAM_PARSE_PEGTLPARSER_HPP
#define HAM_PARSE_PEGTLPARSER_HPP

#include "parse/Grammar.hpp"
#include "parse/Parser.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"
#include "tao/pegtl/contrib/parse_tree_to_dot.hpp"
#include "tao/pegtl/contrib/print.hpp"
#include "tao/pegtl/contrib/trace.hpp"
#include "tao/pegtl/memory_input.hpp"
#include "tao/pegtl/parse_error.hpp"
#include "util/HamError.hpp"

#include <iostream>
#include <string>

namespace ham::parse
{

class PegtlParser {
  public:
	template<typename Rule, typename Input>
	static std::unique_ptr<p::parse_tree::node> Parse(Input&& in)
	{
		try {
			auto node =
				p::parse_tree::parse<Rule, Selector, p::nothing, Control>(in);
			if (node) {
				return node;
			} else {
				return {};
			}
		} catch (p::parse_error err) {
			throw ConvertToHamError(err);
		}
	}

	template<typename Rule>
	static void DebugParse(std::string str, Parser::Debug debug)
	{
		try {
			switch (debug) {
				case Parser::GRAMMAR:
					p::print_debug<Rule>(std::cout);
					break;
				case Parser::XDOT:
					if (auto node = Parse<Rule>(p::memory_input{str, "debug"}))
						p::parse_tree::print_dot(std::cout, *node);
					break;
				case Parser::TRACE:
					p::standard_trace<Rule>(p::memory_input{str, "debug"});
					break;
			}
		} catch (p::parse_error err) {
			throw ConvertToHamError(err);
		}
	}

  private:
	static HamError ConvertToHamError(tao::pegtl::parse_error);
};

} // namespace ham::parse

#endif // HAM_PARSE_PEGTLPARSER_HPP
