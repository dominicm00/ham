/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_PARSER_PARSE_EXCEPTION_HPP
#define HAM_PARSER_PARSE_EXCEPTION_HPP

#include <string>

#include "parser/ParsePosition.hpp"
#include "util/TextFileException.hpp"

namespace ham::parser
{

class ParseException : public util::TextFileException
{
  public:
	ParseException(const std::string& message = std::string(),
				   const std::string& fileName = std::string(),
				   const ParsePosition& position = ParsePosition())
		: util::TextFileException(message,
								  util::TextFilePosition(fileName,
														 position.Line(),
														 position.Column()))
	{
	}
};

} // namespace ham::parser

#endif // HAM_PARSER_PARSE_EXCEPTION_HPP
