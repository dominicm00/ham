/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_GRAMMAR_LEXER_H
#define HAM_GRAMMAR_LEXER_H


#include "grammar/Spirit.h"
	// precompiled -- must be first

#include <stdexcept>

#include "grammar/Token.h"


namespace grammar {


namespace lex = boost::spirit::lex;
namespace qi = boost::spirit::qi;


template<typename BaseIterator> struct TokenIterator;


template<typename BaseIterator>
struct Lexer {
	typedef Token TokenType;
	typedef TokenType value_type;
	typedef TokenIterator<BaseIterator> iterator_type;

	enum ScanMode {
		SCAN_NORMAL,
		SCAN_ACTIONS
	};

	enum KeywordMode {
		KEYWORDS_ON,
		KEYWORDS_OFF
	};

public:
	Lexer(const BaseIterator& start, const BaseIterator& end)
		:
		fPosition(start),
		fEnd(end),
		fCurrentTokenID(0),
		fScanActions(false),
		fIgnoreKeywords(false)
	{
		fString.id(TOKEN_STRING);
		fString.collect(*this, std::string("string"));

		fKeywords.add
			("case", TOKEN_CASE)
			("local", TOKEN_LOCAL)
			("include", TOKEN_INCLUDE)
			("jumptoeof", TOKEN_JUMPTOEOF)
			("on", TOKEN_ON)
			("break", TOKEN_BREAK)
			("continue", TOKEN_CONTINUE)
			("return", TOKEN_RETURN)
			("for", TOKEN_FOR)
			("in", TOKEN_IN)
			("switch", TOKEN_SWITCH)
			("if", TOKEN_IF)
			("else", TOKEN_ELSE)
			("while", TOKEN_WHILE)
			("rule", TOKEN_RULE)
			("actions", TOKEN_ACTIONS)
		;
	}

	size_t add_state(const char* name)
	{
		// Just to satisfy the lexer interface. Not needed.
		return 0;
	}

	template<typename A, typename B>
	size_t add_token(const A&, const B&, size_t tokenID)
	{
		// Just to satisfy the lexer interface. Not needed.
		return tokenID;
	}

	size_t get_next_id()
	{
		// Just to satisfy the lexer interface. Not needed.
		return 0;
	}

	size_t NextToken(size_t id)
	{
		if (id == fCurrentTokenID)
			return _IsEndOfInput() ? kEndTokenID : id + 1;

		if (id == fCurrentTokenID - 1)
			return fCurrentTokenID;

		if (id == kEndTokenID)
			return kEndTokenID;

//		throw std::invalid_argument(
//			std::string("Lexer::NextToken(): Obsolete token ID ") + id
//				+ ", current ID is: " + fCurrentTokenID);
		throw std::invalid_argument(
			"Lexer::NextToken(): Obsolete token ID");
	}

	value_type& CurrentToken(size_t id)
	{
		if (id == fCurrentTokenID)
			return fCurrentToken;

		if (id == fCurrentTokenID + 1) {
			if (_ReadNextToken()) {
				_SkipWhiteSpace();
				return fCurrentToken;
			}
		}

//		throw std::invalid_argument(
//			std::string("Lexer::CurrentToken(): Invalid token ID ") + id
//				+ ", current ID is: " + fCurrentTokenID);
		throw std::invalid_argument(
			"Lexer::CurrentToken(): Invalid token ID");
	}

	inline iterator_type begin();
	inline iterator_type end();

	Lexer& operator=(KeywordMode mode)
	{
		fIgnoreKeywords = mode == KEYWORDS_OFF;
printf("setting keyword mode: %s\n", fIgnoreKeywords ? "off" : "on");
		return *this;
	}

	Lexer& operator=(ScanMode mode)
	{
		fScanActions = mode == SCAN_ACTIONS;
printf("setting scan mode: %s\n", fScanActions ? "actions" : "normal");
		return *this;
	}

private:
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

	bool _ReadNextToken()
	{
		if (fScanActions)
			return _ReadActions();

		while (fPosition != fEnd && isspace(*fPosition))
			++fPosition;

		if (fPosition == fEnd)
			return false;

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
					throw std::invalid_argument(
						"Lexer::_ReadNextToken(): Backslash at end of file");
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
					throw std::invalid_argument(
						"Lexer::_ReadNextToken(): Unterminated string literal");
				}

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
				} else if (!fIgnoreKeywords)
					tokenID = fKeywords.at(token);
			} else if (!fIgnoreKeywords)
				tokenID = fKeywords.at(token);
		}

printf("_ReadNextToken(): read token %d: '%s'\n", tokenID, token.Data());
		fCurrentToken.SetTo(tokenID, token);
		fCurrentTokenID++;

		return true;
	}

	bool _ReadActions()
	{
		// read until we find an unmatched closing brace
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

		if (braces >= 0) {
			throw std::invalid_argument(
				"Lexer::_ReadActions(): Unterminated actions");
		}

printf("_ReadActions(): read: '%s'\n", token.Data());
		fCurrentToken.SetTo(TOKEN_ACTIONS_TEXT, token);
		fCurrentTokenID++;

		return true;
	}

public:
	lex::token_def<String> fString;

private:
//	static const size_t kEndTokenID = std::numeric_limits<size_t>::max();
	static const size_t kEndTokenID = SIZE_MAX;

	BaseIterator	fPosition;
	BaseIterator	fEnd;
	Token			fCurrentToken;
	size_t			fCurrentTokenID;
	qi::symbols<char, TokenID> fKeywords;
	bool			fScanActions;
	bool			fIgnoreKeywords;
};


template<typename BaseIterator>
struct TokenIterator {
	typedef Lexer<BaseIterator> LexerType;

	// standard iterator typedefs
	typedef std::forward_iterator_tag iterator_category;
	typedef BaseIterator base_iterator_type;
	typedef typename LexerType::value_type value_type;
	typedef ssize_t difference_type;
	typedef ssize_t distance_type;
	typedef value_type& reference;
	typedef value_type* pointer;

public:
	TokenIterator()
		:
		fLexer(NULL),
		fID(0)
	{
	}

	TokenIterator(LexerType* lexer, size_t id)
		:
		fLexer(lexer),
		fID(id)
	{
	}

	TokenIterator(const TokenIterator& other)
		:
		fLexer(other.fLexer),
		fID(other.fID)
	{
	}

	TokenIterator& operator=(const TokenIterator& other)
	{
		fLexer = other.fLexer;
		fID = other.fID;
		return *this;
	}

	bool operator==(const TokenIterator& other) const
	{
		return fLexer == other.fLexer && fID == other.fID;
	}

	bool operator!=(const TokenIterator& other) const
	{
		return !(*this == other);
	}

	bool operator<(const TokenIterator& other) const
	{
		if (fLexer != other.fLexer)
			return fLexer < other.fLexer;
		return fID < other.fID;
	}

	bool operator>(const TokenIterator& other) const
	{
		return other < *this;
	}

	bool operator<=(const TokenIterator& other) const
	{
		return !(other < *this);
	}

	bool operator>=(const TokenIterator& other) const
	{
		return !(*this < other);
	}

	TokenIterator& operator++()
	{
		if (fLexer != NULL)
			fID = fLexer->NextToken(fID);
		return *this;
	}

	TokenIterator operator++(int)
	{
		TokenIterator result = *this;
		++*this;
		return result;
	}

	reference operator*() const
	{
		if (fLexer == NULL)
			throw std::invalid_argument("Dereferencing invalid TokenIterator");
		return fLexer->CurrentToken(fID);
	}
	pointer operator->() const
	{
		return &(operator*());
	}

private:
	LexerType*	fLexer;
	size_t		fID;
};


template<typename BaseIterator>
typename Lexer<BaseIterator>::iterator_type
Lexer<BaseIterator>::begin()
{
	_SkipWhiteSpace();

	if (_IsEndOfInput())
		return end();

	return iterator_type(this, fCurrentTokenID == 0 ? 1 : fCurrentTokenID);
}


template<typename BaseIterator>
typename Lexer<BaseIterator>::iterator_type
Lexer<BaseIterator>::end()
{
	return iterator_type(this, kEndTokenID);
}


} // namespace grammar


#endif	// HAM_GRAMMAR_TOKEN_H
