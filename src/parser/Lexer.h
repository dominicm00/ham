/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_PARSER_LEXER_H
#define HAM_PARSER_LEXER_H


#include <map>
#include <stdexcept>

#include "parser/LexException.h"
#include "parser/Token.h"


namespace parser {


template<typename BaseIterator> struct TokenIterator;


template<typename BaseIterator>
class Lexer {
public:
	Lexer()
	{
		_Init();
	}

	void Init(const BaseIterator& start, const BaseIterator& end)
	{
		fPosition = start;
		fEnd = end;

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
			throw LexException("Unterminated actions");

printf("ScanActions(): read: '%s'\n", token.Data());
		return data::String(token.Data());
	}

private:
	typedef std::map<std::string, TokenID> KeywordMap;

private:
	void _Init()
	{
		// TODO: Use a better map.
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

		if (fPosition == fEnd) {
			fCurrentToken.SetTo(TOKEN_EOF, std::string());
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

				if (fPosition == fEnd)
					throw LexException("Backslash at end of file");

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

				if (!foundEnd)
					throw LexException("Unterminated string literal");

				continue;
			}

			token += c;
		}

		TokenID tokenID = TOKEN_STRING;

		if (!quotedOrEscaped) {
			if (token.Length() == 1) {
				switch (token[0]) {
					case ';':
						tokenID = TOKEN_SEMICOLON;
						break;
					case ':':
						tokenID = TOKEN_COLON;
						break;
					case '[':
						tokenID = TOKEN_LEFT_BRACKET;
						break;
					case ']':
						tokenID = TOKEN_RIGHT_BRACKET;
						break;
					case '{':
						tokenID = TOKEN_LEFT_BRACE;
						break;
					case '}':
						tokenID = TOKEN_RIGHT_BRACE;
						break;
					case '(':
						tokenID = TOKEN_LEFT_PARENTHESIS;
						break;
					case ')':
						tokenID = TOKEN_RIGHT_PARENTHESIS;
						break;
					case '=':
						tokenID = TOKEN_ASSIGN;
						break;
					case '|':
						tokenID = TOKEN_OR;
						break;
					case '&':
						tokenID = TOKEN_AND;
						break;
					case '<':
						tokenID = TOKEN_LESS;
						break;
					case '>':
						tokenID = TOKEN_GREATER;
						break;
					case '!':
						tokenID = TOKEN_NOT;
						break;
				}
			} else if (token.Length() == 2) {
				if (token[1] == '=') {
					switch (token[0]) {
						case '+':
							tokenID = TOKEN_ASSIGN_PLUS;
							break;
						case '?':
							tokenID = TOKEN_ASSIGN_DEFAULT;
							break;
						case '!':
							tokenID = TOKEN_NOT_EQUAL;
							break;
						case '<':
							tokenID = TOKEN_LESS_OR_EQUAL;
							break;
						case '>':
							tokenID = TOKEN_GREATER_OR_EQUAL;
							break;
					}
				} else {
					KeywordMap::iterator it = fKeywords.find(token);
					if (it != fKeywords.end())
						tokenID = it->second;
				}
			} else {
				KeywordMap::iterator it = fKeywords.find(token);
				if (it != fKeywords.end())
					tokenID = it->second;
			}
		}

printf("_ReadNextToken(): read token %d: '%s'\n", tokenID, token.Data());
		fCurrentToken.SetTo(tokenID, token);
	}

private:
	BaseIterator	fPosition;
	BaseIterator	fEnd;
	Token			fCurrentToken;
	KeywordMap		fKeywords;
};


} // namespace parser


#endif	// HAM_PARSER_TOKEN_H
