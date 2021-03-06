/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_PARSER_PARSER_HPP
#define HAM_PARSER_PARSER_HPP

#include "code/Defs.hpp"
#include "parser/Lexer.hpp"

#include <iterator>

namespace ham
{

namespace code
{
class Block;
class List;
class Node;
} // namespace code

namespace parser
{

class Parser
{
  public:
	class Listener;

	typedef std::istream_iterator<char> InputIteratorType;

  public:
	Parser();

	void SetFileName(const std::string& fileName) { fFileName = fileName; }
	// used for exceptions only

	code::Block* Parse(const std::string& input);
	code::Block* Parse(std::istream& input);
	code::Block*
	Parse(const InputIteratorType& start, const InputIteratorType& end);
	code::Block* ParseFile(const char* fileName);

	void Test(int argc, const char* const* argv);

  private:
	typedef parser::Lexer<InputIteratorType> LexerType;

	class NodeListContainer;
	class ListenerNotifier;
	class DumpListener;

  private:
	code::Block* _ParseFile();
	code::Block* _ParseBlock();
	code::Node* _TryParseStatement();
	code::Node* _ParseLocalVariableDeclaration();
	code::List* _ParseList();
	code::Node* _TryParseArgument(bool allowKeyword = false);
	code::Node* _ParseBracketExpression();
	code::Node* _TryParseBracketOnExpression();
	code::Node* _TryParseFunctionCall();
	void _ParseListOfLists(NodeListContainer& nodes);
	code::Node* _ParseExpression();
	code::Node* _ParseAndExpression();
	code::Node* _ParseComparison();
	code::Node* _ParseAtom();

	inline code::AssignmentOperator _ParseAssignmentOperator();

	void _Throw(const char* message);
	void _ThrowExpected(const char* expected);

	const Token& _Token() const { return fLexer.CurrentToken(); }
	inline const Token& _NextToken();
	inline bool _TrySkipToken(TokenID id);
	inline void _SkipToken(TokenID id, const char* expected);

	template<typename NodeType>
	inline NodeType* _Expect(NodeType* node, const char* expected);

  private:
	LexerType fLexer;
	Listener* fListener;
	std::string fFileName;
};

class Parser::Listener
{
  public:
	virtual ~Listener();

	virtual void NonterminalStart(const char* name);
	virtual void NonterminalEnd(const char* name);
	virtual void NextToken(const Token& token);
};

const Token&
Parser::_NextToken()
{
	const Token& token = fLexer.NextToken();

	if (fListener != nullptr)
		fListener->NextToken(token);

	return token;
}

bool
Parser::_TrySkipToken(TokenID id)
{
	if (_Token() != id)
		return false;

	_NextToken();
	return true;
}

void
Parser::_SkipToken(TokenID id, const char* expected)
{
	if (!_TrySkipToken(id))
		_ThrowExpected(expected);
}

template<typename NodeType>
NodeType*
Parser::_Expect(NodeType* node, const char* expected)
{
	if (node == nullptr)
		_ThrowExpected(expected);
	return node;
}

} // namespace parser
} // namespace ham

#endif // HAM_PARSER_PARSER_HPP
