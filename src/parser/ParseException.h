/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_PARSER_PARSE_EXCEPTION_H
#define HAM_PARSER_PARSE_EXCEPTION_H


#include <string>


namespace parser {


struct ParseLocation {
};


class ParseException {
public:
	ParseException()
	{
	}

	ParseException(const std::string& message,
		const ParseLocation& location = ParseLocation())
		:
		fMessage(message)
	{
	}

	const char* Message() const
	{
		return fMessage.c_str();
	}

	const ParseLocation& Location() const
	{
		return fLocation;
	}

private:
	std::string		fMessage;
	ParseLocation	fLocation;
};


} // namespace parser


#endif	// HAM_PARSER_PARSE_EXCEPTION_H
