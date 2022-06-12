/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_PARSER_PARSE_EXCEPTION_H
#define HAM_PARSER_PARSE_EXCEPTION_H

#include <string>

#include "parser/ParsePosition.h"
#include "util/TextFileException.h"

namespace ham
{
namespace parser
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

} // namespace parser
} // namespace ham

#endif // HAM_PARSER_PARSE_EXCEPTION_H
