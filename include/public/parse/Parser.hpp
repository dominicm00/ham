#ifndef HAM_PARSE_PARSER_HPP
#define HAM_PARSE_PARSER_HPP

#import <string>

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
	static void Parse(std::string in, Debug debug);
};

} // namespace ham::parse

#endif // HAM_PARSE_PARSER_HPP
