/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_PARSER_LEX_EXCEPTION_H
#define HAM_PARSER_LEX_EXCEPTION_H


#include <string>

#include "parser/ParsePosition.h"


namespace ham {
namespace parser {


class LexException {
public:
	LexException()
	{
	}

	LexException(const std::string& message,
		const ParsePosition& position = ParsePosition())
		:
		fMessage(message),
		fPosition(position)
	{
	}

	const char* Message() const
	{
		return fMessage.c_str();
	}

	const ParsePosition& Position() const
	{
		return fPosition;
	}

private:
	std::string		fMessage;
	ParsePosition	fPosition;
};


}	// namespace parser
}	// namespace ham


#endif	// HAM_PARSER_LEX_EXCEPTION_H
