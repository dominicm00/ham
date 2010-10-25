/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_PARSER_TOKEN_H
#define HAM_PARSER_TOKEN_H


#include "data/String.h"


namespace parser {


enum TokenID {
	TOKEN_STRING = 0,

	// delimiters/operators
	TOKEN_SEMICOLON,
	TOKEN_COLON,
	TOKEN_LEFT_BRACKET,
	TOKEN_RIGHT_BRACKET,
	TOKEN_LEFT_BRACE,
	TOKEN_RIGHT_BRACE,
	TOKEN_LEFT_PARENTHESIS,
	TOKEN_RIGHT_PARENTHESIS,
	TOKEN_ASSIGN,
	TOKEN_EQUAL = TOKEN_ASSIGN,
	TOKEN_ASSIGN_PLUS,
	TOKEN_ASSIGN_DEFAULT,
	TOKEN_OR,
	TOKEN_AND,
	TOKEN_NOT_EQUAL,
	TOKEN_LESS,
	TOKEN_LESS_OR_EQUAL,
	TOKEN_GREATER,
	TOKEN_GREATER_OR_EQUAL,
	TOKEN_NOT,

	TOKEN_DELIMITERS_END,

	// keywords
	TOKEN_ACTIONS = TOKEN_DELIMITERS_END,
	TOKEN_BIND,
	TOKEN_BREAK,
	TOKEN_CASE,
	TOKEN_CONTINUE,
	TOKEN_ELSE,
	TOKEN_FOR,
	TOKEN_IF,
	TOKEN_IN,
	TOKEN_INCLUDE,
	TOKEN_JUMPTOEOF,
	TOKEN_LOCAL,
	TOKEN_ON,
	TOKEN_RETURN,
	TOKEN_RULE,
	TOKEN_SWITCH,
	TOKEN_WHILE,

	TOKEN_KEYWORDS_END,

	// the text within an actions block
	TOKEN_ACTIONS_TEXT = TOKEN_KEYWORDS_END,

	// end of file
	TOKEN_EOF
};


struct Token : data::String {
	typedef TokenID id_type;

public:
	Token()
		:
		fID(TOKEN_STRING)
	{
	}

	Token(TokenID id, const data::String& value)
		:
		data::String(value),
		fID(id)
	{
	}

	void SetTo(TokenID id, const data::String& value)
	{
		String::operator=(value);
		fID = id;
	}

	TokenID ID() const
	{
		return fID;
	}

	bool IsKeyword() const
	{
		return fID >= TOKEN_DELIMITERS_END && fID < TOKEN_KEYWORDS_END;
	}

	bool IsStringOrKeyword() const
	{
		return fID == TOKEN_STRING || IsKeyword();
	}

	bool operator==(TokenID id) const
	{
		return fID == id;
	}

	bool operator!=(TokenID id) const
	{
		return !(*this == id);
	}

	template<typename Stream>
	friend
	Stream& operator<<(Stream& stream, const Token& token)
	{
		stream << '(' << token.fID << ", \"" << (const String&)token << "\")";
		return stream;
	}

private:
	TokenID	fID;
};


} // namespace parser


#endif	// HAM_PARSER_TOKEN_H
