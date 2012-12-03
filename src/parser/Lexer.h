/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_PARSER_LEXER_H
#define HAM_PARSER_LEXER_H


#include <map>
#include <stdexcept>

#include "data/StringBuffer.h"
#include "parser/LexException.h"
#include "parser/Token.h"


//#define TRACE_PARSER_LEXER
#ifdef TRACE_PARSER_LEXER
#	define TRACE(...)	printf(__VA_ARGS__)
#else
#	define TRACE(...)	do { } while (false)
#endif


namespace ham {
namespace parser {


template<typename BaseIterator> struct TokenIterator;


template<typename BaseIterator>
class Lexer {
private:
	struct Iterator {
		Iterator()
			:
			fLine(0),
			fColumn(0)
		{
		}

		size_t Line() const
		{
			return fLine;
		}

		size_t Column() const
		{
			return fColumn;
		}

		Iterator& operator=(const BaseIterator& iterator)
		{
			fIterator = iterator;
			fLine = 0;
			fColumn = 0;
			return *this;
		}

		bool operator==(const Iterator& other) const
		{
			return fIterator == other.fIterator;
		}

		bool operator==(const BaseIterator& iterator) const
		{
			return fIterator == iterator;
		}

		bool operator!=(const Iterator& other) const
		{
			return !(*this == other);
		}

		bool operator!=(const BaseIterator& iterator) const
		{
			return !(*this == iterator);
		}

		Iterator& operator++()
		{
			if (*fIterator == '\n') {
				fLine++;
				fColumn = 0;
			} else
				fColumn++;

			++fIterator;
			return *this;
		}

		char operator*()
		{
			return *fIterator;
		}

	private:
		BaseIterator	fIterator;
		size_t			fLine;
		size_t			fColumn;
	};

public:
	Lexer()
	{
		_Init();
	}

	void Init(const BaseIterator& start, const BaseIterator& end)
	{
		fPosition = start;
		fEnd = end;

		fFilePosition.SetTo(0, 0);

		_ReadNextToken();
	}

	const Token& NextToken()
	{
		_ReadNextToken();
		return fCurrentToken;
	}

	const Token& CurrentToken() const
	{
		return fCurrentToken;
	}

	const ParsePosition& CurrentTokenPosition() const
	{
		return fFilePosition;
	}

	data::String ScanActions()
	{
		// read until we find an unmatched closing brace
		// TODO: This algorithm needs to be improved! We don't recognize braces
		// in comments, strings, or quoted braces.
		data::StringBuffer token;
		int braces = 0;
		while (fPosition != fEnd) {
			char c = *fPosition;
			if (c == '{') {
				braces++;
			} else if (c == '}') {
				if (--braces < 0)
					break;
			}

			token += c;
			++fPosition;
		}

		if (braces >= 0)
			throw LexException("Unterminated actions", fFilePosition);

		TRACE("ScanActions(): read: '%s'\n", token.Data());

		return data::String(token.Data());
	}

private:
	typedef std::map<data::String, TokenID> KeywordMap;

private:
	void _Init()
	{
		// TODO: Use a better map.
		fKeywords[";"] = TOKEN_SEMICOLON;
		fKeywords[":"] = TOKEN_COLON;
		fKeywords["["] = TOKEN_LEFT_BRACKET;
		fKeywords["]"] = TOKEN_RIGHT_BRACKET;
		fKeywords["{"] = TOKEN_LEFT_BRACE;
		fKeywords["}"] = TOKEN_RIGHT_BRACE;
		fKeywords["("] = TOKEN_LEFT_PARENTHESIS;
		fKeywords[")"] = TOKEN_RIGHT_PARENTHESIS;
		fKeywords["="] = TOKEN_ASSIGN;
		fKeywords["+="] = TOKEN_ASSIGN_PLUS;
		fKeywords["?="] = TOKEN_ASSIGN_DEFAULT;
		fKeywords["|"] = TOKEN_OR;
		fKeywords["||"] = TOKEN_OR;
		fKeywords["&"] = TOKEN_AND;
		fKeywords["&&"] = TOKEN_AND;
		fKeywords["!="] = TOKEN_NOT_EQUAL;
		fKeywords["<"] = TOKEN_LESS;
		fKeywords["<="] = TOKEN_LESS_OR_EQUAL;
		fKeywords[">"] = TOKEN_GREATER;
		fKeywords[">="] = TOKEN_GREATER_OR_EQUAL;
		fKeywords["!"] = TOKEN_NOT;

		fKeywords["actions"] = TOKEN_ACTIONS;
		fKeywords["bind"] = TOKEN_BIND;
		fKeywords["break"] = TOKEN_BREAK;
		fKeywords["case"] = TOKEN_CASE;
		fKeywords["continue"] = TOKEN_CONTINUE;
		fKeywords["else"] = TOKEN_ELSE;
		fKeywords["for"] = TOKEN_FOR;
		fKeywords["if"] = TOKEN_IF;
		fKeywords["in"] = TOKEN_IN;
		fKeywords["include"] = TOKEN_INCLUDE;
		fKeywords["jumptoeof"] = TOKEN_JUMPTOEOF;
		fKeywords["local"] = TOKEN_LOCAL;
		fKeywords["on"] = TOKEN_ON;
		fKeywords["return"] = TOKEN_RETURN;
		fKeywords["rule"] = TOKEN_RULE;
		fKeywords["switch"] = TOKEN_SWITCH;
		fKeywords["while"] = TOKEN_WHILE;
	}

	void _SkipWhiteSpace()
	{
		while (true) {
			// skip whitespace
			while (fPosition != fEnd && isspace(*fPosition))
				++fPosition;

			if (fPosition == fEnd || *fPosition != '#')
				break;

			// skip comment
			while (fPosition != fEnd && *fPosition != '\n')
				++fPosition;
		}
	}

	bool _IsEndOfInput() const
	{
		return fPosition == fEnd;
	}

	void _ReadNextToken()
	{
		_SkipWhiteSpace();

		fFilePosition.SetTo(fPosition.Line(), fPosition.Column());

		if (fPosition == fEnd) {
			fCurrentToken.SetTo(TOKEN_EOF, data::String());
			return;
		}

		bool quotedOrEscaped = false;

		data::StringBuffer token;
		while (fPosition != fEnd) {
			char c = *fPosition;
			if (isspace(c))
				break;

			++fPosition;

			if (c == '\\') {
				// escaped char
				quotedOrEscaped = true;

				if (fPosition == fEnd) {
					throw LexException("Backslash at end of file",
						fFilePosition);
				}

				// fetch the escaped char
				c = *fPosition;
				++fPosition;
			} else if (c == '"') {
				// quoted (sub)string
				quotedOrEscaped = true;

				// loop until we find the terminating quotes
				bool foundEnd = false;
				while (fPosition != fEnd) {
					c = *fPosition;
					++fPosition;

					if (c == '"') {
						foundEnd = true;
						break;
					}

					if (c == '\\') {
						// escaped char
						if (fPosition == fEnd) {
							// will throw after the loop
							break;
						}

						c = *fPosition;
						++fPosition;
					}

					token += c;
				}

				if (!foundEnd) {
					throw LexException("Unterminated string literal",
						fFilePosition);
				}

				continue;
			}

			token += c;
		}

		TokenID tokenID = TOKEN_STRING;

		if (!quotedOrEscaped) {
			KeywordMap::iterator it = fKeywords.find(token);
			if (it != fKeywords.end())
				tokenID = it->second;
		}

		TRACE("_ReadNextToken(): read token %d: '%s'\n", tokenID, token.Data());

		fCurrentToken.SetTo(tokenID, token);
	}

private:
	Iterator		fPosition;
	BaseIterator	fEnd;
	Token			fCurrentToken;
	KeywordMap		fKeywords;
	ParsePosition	fFilePosition;
};


}	// namespace parser
}	// namespace ham


#undef TRACE


#endif	// HAM_PARSER_TOKEN_H
