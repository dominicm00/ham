/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_GRAMMAR_PARSER_H
#define HAM_GRAMMAR_PARSER_H


#include <iterator>

#include "code/Defs.h"
#include "grammar/Lexer.h"


namespace code {
	class Block;
	class List;
	class Node;
}


namespace grammar {


class Parser {
public:
								Parser();

			void				Test(int argc, const char* const* argv);

private:
			typedef std::istream_iterator<char> BaseIteratorType;
			typedef grammar::Lexer<BaseIteratorType> LexerType;

			struct NodeListContainer;

private:
			code::Block*		_ParseFile();
			code::Block*		_ParseBlock();
			code::Node*			_TryParseStatement();
			code::Node*			_TryParseLocalVariableDeclaration();
			code::List*			_ParseList();
			code::Node*			_TryParseArgument();
			code::Node*			_ParseBracketExpression();
			code::Node*			_TryParseBracketOnExpression();
			code::Node*			_TryParseFunctionCall();
			void				_ParseListOfLists(NodeListContainer& nodes);
			code::Node*			_ParseExpression();
			code::Node*			_ParseAtom();

	inline	code::AssignmentOperator _ParseAssignmentOperator();

			void				_Throw(const char* message);

			const Token&		_Token() const
									{ return fLexer.CurrentToken(); }
			const Token&		_NextToken()
									{ return fLexer.NextToken(); }
	inline	bool				_TrySkipToken(TokenID id);
	inline	void				_SkipToken(TokenID id, const char* message);

	template<typename NodeType>
	inline	NodeType*			_Expect(NodeType* node, const char* message);

private:
			LexerType			fLexer;
};


bool
Parser::_TrySkipToken(TokenID id)
{
	if (_Token() != id)
		return false;

	_NextToken();
	return true;
}


void
Parser::_SkipToken(TokenID id, const char* message)
{
	if (!_TrySkipToken(id))
		_Throw(message);
}


template<typename NodeType>
NodeType*
Parser::_Expect(NodeType* node, const char* message)
{
	if (node == NULL)
		_Throw(message);
	return node;
}


} // namespace grammar


#endif	// HAM_GRAMMAR_PARSER_H
