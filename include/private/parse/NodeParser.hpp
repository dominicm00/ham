#ifndef HAM_PARSE_NODEPARSER_HPP
#define HAM_PARSE_NODEPARSER_HPP

#include "HamError.hpp"
#include "parse/Grammar.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"
#include "tao/pegtl/contrib/parse_tree_to_dot.hpp"
#include "tao/pegtl/contrib/print.hpp"
#include "tao/pegtl/contrib/trace.hpp"
#include "tao/pegtl/memory_input.hpp"
#include "tao/pegtl/parse_error.hpp"

#include <iostream>
#include <string>

namespace ham::parse
{

namespace p = tao::pegtl;

class NodeParser {
  public:
	enum Debug {
		XDOT,
		TRACE,
		GRAMMAR
	};

  public:
	template<typename Rule, typename Input>
	static std::unique_ptr<p::parse_tree::node> ParseNode(Input&& in)
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
	static void DebugParse(std::string str, Debug debug)
	{
		try {
			switch (debug) {
				case GRAMMAR:
					p::print_debug<Rule>(std::cout);
					break;
				case XDOT:
					if (auto node =
							ParseNode<Rule>(p::memory_input{str, "debug"}))
						p::parse_tree::print_dot(std::cout, *node);
					break;
				case TRACE:
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

#endif // HAM_PARSE_NODEPARSER_HPP
