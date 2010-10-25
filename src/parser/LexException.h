/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_PARSER_LEX_EXCEPTION_H
#define HAM_PARSER_LEX_EXCEPTION_H


#include <string>


namespace parser {


struct LexLocation {
};


class LexException {
public:
	LexException()
	{
	}

	LexException(const std::string& message,
		const LexLocation& location = LexLocation())
		:
		fMessage(message)
	{
	}

	const char* Message() const
	{
		return fMessage.c_str();
	}

	const LexLocation& Location() const
	{
		return fLocation;
	}

private:
	std::string	fMessage;
	LexLocation	fLocation;
};


} // namespace parser


#endif	// HAM_PARSER_LEX_EXCEPTION_H
