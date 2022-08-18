#ifndef HAM_PARSE_PARSER_HPP
#define HAM_PARSE_PARSER_HPP

#include "tao/pegtl/contrib/parse_tree.hpp"

#include <string>

namespace ham::parse
{

class NodeParser {
  public:
	enum Debug {
		NONE,
		XDOT,
		TRACE,
		GRAMMAR
	};

  public:
	template<typename Rule, typename Input>
	tao::pegtl::parse_tree::node Parse(Input in);
};

} // namespace ham::parse

#endif // HAM_PARSE_PARSER_HPP
