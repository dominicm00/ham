#ifndef HAM_PARSE_PARSER_HPP
#define HAM_PARSE_PARSER_HPP

#include "code/Node.hpp"

#include <string>

namespace ham::parse
{

class Parser {
  public:
	enum Debug {
		XDOT,
		TRACE,
		GRAMMAR
	};

  public:
	// static code::Node Parse(std::string in);
};

} // namespace ham::parse

#endif // HAM_PARSE_PARSER_HPP
