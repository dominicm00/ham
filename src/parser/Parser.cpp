/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "parser/Parser.hpp"

#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "code/ActionsDefinition.hpp"
#include "code/Assignment.hpp"
#include "code/BinaryExpression.hpp"
#include "code/Block.hpp"
#include "code/Case.hpp"
#include "code/DumpContext.hpp"
#include "code/For.hpp"
#include "code/FunctionCall.hpp"
#include "code/If.hpp"
#include "code/InListExpression.hpp"
#include "code/Include.hpp"
#include "code/Jump.hpp"
#include "code/Leaf.hpp"
#include "code/List.hpp"
#include "code/LocalVariableDeclaration.hpp"
#include "code/NotExpression.hpp"
#include "code/OnExpression.hpp"
#include "code/RuleDefinition.hpp"
#include "code/Switch.hpp"
#include "code/While.hpp"
#include "data/RuleActions.hpp"
#include "parser/ParseException.hpp"

namespace ham
{
namespace parser
{

namespace
{
struct ActionsFlagMap : std::map<data::String, uint32_t> {
	ActionsFlagMap()
	{
		(*this)["updated"] = data::RuleActions::UPDATED;
		(*this)["together"] = data::RuleActions::TOGETHER;
		(*this)["ignore"] = data::RuleActions::IGNORE;
		(*this)["quietly"] = data::RuleActions::QUIETLY;
		(*this)["piecemeal"] = data::RuleActions::PIECEMEAL;
		(*this)["existing"] = data::RuleActions::EXISTING;
		(*this)["maxline"] = data::RuleActions::MAX_LINE_FACTOR;
	}
};

static ActionsFlagMap sActionsFlags;
}

// #pragma mark - Listener

Parser::Listener::~Listener() {}

void
Parser::Listener::NonterminalStart(const char* name)
{
}

void
Parser::Listener::NonterminalEnd(const char* name)
{
}

void
Parser::Listener::NextToken(const Token& token)
{
}

// #pragma mark - NodeListContainer

struct Parser::NodeListContainer {
	code::NodeList fNodes;

	~NodeListContainer()
	{
		for (code::NodeList::iterator it = fNodes.begin(); it != fNodes.end();
			 ++it) {
			(*it)->ReleaseReference();
		}
	}

	void Add(code::Node* node)
	{
		fNodes.push_back(node);
		node->AcquireReference();
	}

	void Detach() { fNodes.clear(); }
};

// #pragma mark - ListenerNotifier

struct Parser::ListenerNotifier {
	ListenerNotifier(Parser::Listener* listener, const char* name)
		: fListener(listener)
	{
		if (fListener != nullptr) {
			fName = name;
			fListener->NonterminalStart(fName);
		}
	}

	~ListenerNotifier()
	{
		if (fListener != nullptr)
			fListener->NonterminalEnd(fName);
	}

  private:
	Parser::Listener* fListener;
	const char* fName;
};

#define PARSER_NONTERMINAL(name) ListenerNotifier _notifier(fListener, name);

// #pragma mark - DumpListener

struct Parser::DumpListener : Parser::Listener {
	DumpListener()
		: fLevel(0)
	{
	}

	virtual void NonterminalStart(const char* name)
	{
		printf("%*s<%s>\n", (int)fLevel * 2, "", name);
		fLevel++;
	}

	virtual void NonterminalEnd(const char* name)
	{
		fLevel--;
		printf("%*s</%s>\n", (int)fLevel * 2, "", name);
	}

	virtual void NextToken(const Token& token)
	{
		printf("%*s(%d, \"%s\")\n",
			   (int)fLevel * 2,
			   "",
			   token.ID(),
			   token.ToCString());
	}

  private:
	size_t fLevel;
};

// #pragma mark - Parser

Parser::Parser()
	: fListener(nullptr)
{
}

code::Block*
Parser::Parse(const std::string& input)
{
	std::istringstream stream(input);
	return Parse(stream);
}

code::Block*
Parser::Parse(std::istream& input)
{
	bool skipWhiteSpace = (input.flags() & std::ios_base::skipws) != 0;
	try {
		std::noskipws(input);
		code::Block* result =
			Parse(InputIteratorType(input), InputIteratorType());
		if (skipWhiteSpace)
			std::skipws(input);
		return result;
	} catch (...) {
		if (skipWhiteSpace)
			std::skipws(input);
		throw;
	}
}

code::Block*
Parser::Parse(const InputIteratorType& start, const InputIteratorType& end)
{
	fLexer.Init(start, end);
	return _ParseFile();
}

code::Block*
Parser::ParseFile(const char* fileName)
{
	std::ifstream input(fileName);
	if (input.fail()) {
		_Throw(
			(std::string("Failed to open file \"") + fileName + "\"").c_str());
	}

	return Parse(input);
}

void
Parser::Test(int argc, const char* const* argv)
{
#if 1
	std::istream& input = std::cin;
#else
	std::ifstream input("testdata/test2");
#endif

	DumpListener dumpListener;

	if (argc > 1 && strcmp(argv[1], "-d") == 0) {
		fListener = &dumpListener;
		fListener->NextToken(_Token());
	}

	try {
		code::Block* block = Parse(input);
		util::Reference<code::Block> blockReference(block, true);
		std::cout << "Parse tree:\n";
		code::DumpContext dumpContext;
		//		block->Dump(dumpContext);
	} catch (LexException& exception) {
		printf("Parser::Test(): %zu:%zu Lex exception: %s\n",
			   exception.Position().Line() + 1,
			   exception.Position().Column() + 1,
			   exception.Message());
	} catch (ParseException& exception) {
		printf("Parser::Test(): %zu:%zu Parse exception: %s\n",
			   exception.Position().Line() + 1,
			   exception.Position().Column() + 1,
			   exception.Message());
	} catch (...) {
		printf("Parser::Test(): Caught exception\n");
	}

	fListener = nullptr;
}

code::Block*
Parser::_ParseFile()
{
	util::Reference<code::Block> block(_ParseBlock(), true);

	if (_Token() != TOKEN_EOF)
		_ThrowExpected("Expected statement or local variable declaration");

	return block.Detach();
}

code::Block*
Parser::_ParseBlock()
{
	PARSER_NONTERMINAL("block");

	util::Reference<code::Block> block(new code::Block, true);

	while (true) {
		// local variable declaration or statement
		if (_Token() == TOKEN_LOCAL)
			block->AppendKeepReference(_ParseLocalVariableDeclaration());
		else if (code::Node* statement = _TryParseStatement())
			block->AppendKeepReference(statement);
		else
			break;
	}

	return block.Detach();
}

code::Node*
Parser::_TryParseStatement()
{
	PARSER_NONTERMINAL("statement");

	switch (_Token().ID()) {
		case TOKEN_LEFT_BRACE: {
			// "{" block "}"
			PARSER_NONTERMINAL("braced block");

			_NextToken();
			code::NodeReference result(_ParseBlock(), true);
			_SkipToken(TOKEN_RIGHT_BRACE,
					   "Expected '}' at the end of the block");
			return result.Detach();
		}

		case TOKEN_INCLUDE: {
			// "include" list ";"
			PARSER_NONTERMINAL("include");

			_NextToken();
			code::NodeReference list(_ParseList(), true);
			_SkipToken(TOKEN_SEMICOLON,
					   "Expected ';' at the end of the 'include' statement");

			// create node
			return new code::Include(list.Get());
		}

		case TOKEN_BREAK: {
			// "break" list ";"
			PARSER_NONTERMINAL("break");

			_NextToken();
			code::NodeReference list(_ParseList(), true);
			_SkipToken(TOKEN_SEMICOLON,
					   "Expected ';' at the end of the 'break' statement");

			// create node
			return new code::Break(list.Get());
		}

		case TOKEN_CONTINUE: {
			// "continue" list ";"
			PARSER_NONTERMINAL("continue");

			_NextToken();
			code::NodeReference list(_ParseList(), true);
			_SkipToken(TOKEN_SEMICOLON,
					   "Expected ';' at the end of the 'continue' statement");

			// create node
			return new code::Continue(list.Get());
		}

		case TOKEN_RETURN: {
			// "return" list ";"
			PARSER_NONTERMINAL("return");

			_NextToken();
			code::NodeReference list(_ParseList(), true);
			_SkipToken(TOKEN_SEMICOLON,
					   "Expected ';' at the end of the 'return' statement");

			// create node
			return new code::Return(list.Get());
		}

		case TOKEN_JUMPTOEOF: {
			// "jumptoeof" list ";"
			PARSER_NONTERMINAL("jumptoeof");

			_NextToken();
			code::NodeReference list(_ParseList(), true);
			_SkipToken(TOKEN_SEMICOLON,
					   "Expected ';' at the end of the 'jumptoeof' statement");

			// create node
			return new code::JumpToEof(list.Get());
		}

		case TOKEN_IF: {
			// "if" expression "{" block "}" [ "else" statement ]
			PARSER_NONTERMINAL("if");

			_NextToken();
			code::NodeReference expression(_ParseExpression(), true);
			_SkipToken(TOKEN_LEFT_BRACE, "Expected '{' after 'if' expression");
			code::NodeReference block(_ParseBlock(), true);
			_SkipToken(TOKEN_RIGHT_BRACE, "Expected '}' after 'if' block");

			// optional 'else' branch
			code::NodeReference elseBlock;
			if (_TrySkipToken(TOKEN_ELSE)) {
				elseBlock.SetTo(_Expect(_TryParseStatement(),
										"Expected statement after 'else'"),
								true);
			}

			// create node
			return new code::If(expression.Get(), block.Get(), elseBlock.Get());
		}

		case TOKEN_FOR: {
			// "for" argument "in" list "{" block "}"
			PARSER_NONTERMINAL("for");

			_NextToken();
			code::NodeReference argument(
				_Expect(_TryParseArgument(), "Expected argument after 'for'"),
				true);
			_SkipToken(TOKEN_IN, "Expected 'in' after 'for' argument");
			code::NodeReference list(_ParseList(), true);
			_SkipToken(TOKEN_LEFT_BRACE, "Expected '{' after 'for' head");
			code::NodeReference block(_ParseBlock(), true);
			_SkipToken(TOKEN_RIGHT_BRACE, "Expected '}' after 'for' block");

			// create node
			return new code::For(argument.Get(), list.Get(), block.Get());
		}

		case TOKEN_WHILE: {
			// "while" expression "{" block "}"
			PARSER_NONTERMINAL("while");

			_NextToken();
			code::NodeReference expression(_ParseExpression(), true);
			_SkipToken(TOKEN_LEFT_BRACE, "Expected '{' after 'while' head");
			code::NodeReference block(_ParseBlock(), true);
			_SkipToken(TOKEN_RIGHT_BRACE, "Expected '}' after 'while' block");

			// create node
			return new code::While(expression.Get(), block.Get());
		}

		case TOKEN_SWITCH: {
			// "switch" list "{" case* "}"
			PARSER_NONTERMINAL("switch");

			_NextToken();
			code::NodeReference list(_ParseList(), true);
			_SkipToken(TOKEN_LEFT_BRACE, "Expected '{' after 'while' head");

			// create node
			util::Reference<code::Switch> switchNode(
				new code::Switch(list.Get()),
				true);

			// each case: "case" identifier ":" block
			while (_TrySkipToken(TOKEN_CASE)) {
				PARSER_NONTERMINAL("case");

				if (_Token() != TOKEN_STRING)
					_ThrowExpected("Expected pattern string after 'case'");

				data::String pattern = _Token();
				_NextToken();
				_SkipToken(TOKEN_COLON, "Expected ':' after 'case' pattern");
				code::NodeReference block(_ParseBlock(), true);

				switchNode->AddCase(pattern, block.Get());
			}

			_SkipToken(TOKEN_RIGHT_BRACE,
					   "Expected '}' at the end of the 'switch' statement");

			return switchNode.Detach();
		}

		case TOKEN_ON: {
			// "on" argument statement
			PARSER_NONTERMINAL("on");

			_NextToken();
			code::NodeReference argument(
				_Expect(_TryParseArgument(), "Expected argument after 'on'"),
				true);
			code::NodeReference statement(
				_Expect(_TryParseStatement(),
						"Expected statement after 'on' argument"),
				true);

			// create node
			return new code::OnExpression(argument.Get(), statement.Get());
		}

		case TOKEN_RULE: {
			// "rule" identifier [ identifier ( ":" identifier )*  ]
			// "{" block "}"
			PARSER_NONTERMINAL("rule definition");

			_NextToken();

			if (_Token() != TOKEN_STRING)
				_ThrowExpected("Expected rule name string after 'rule'");
			data::String ruleName = _Token();
			_NextToken();

			// parse rule parameter names
			StringList parameterNames;
			if (_Token() == TOKEN_STRING) {
				parameterNames.Append(_Token());
				_NextToken();

				while (_TrySkipToken(TOKEN_COLON)) {
					if (_Token() != TOKEN_STRING) {
						_ThrowExpected(
							"Expected 'rule' parameter name after ':'");
					}

					parameterNames.Append(_Token());
					_NextToken();
				}
			}

			// parse block
			_SkipToken(TOKEN_LEFT_BRACE, "Expected '{' after 'while' head");
			util::Reference<code::Block> block(_ParseBlock(), true);
			_SkipToken(TOKEN_RIGHT_BRACE, "Expected '}' after 'while' block");

			// create node
			return new code::RuleDefinition(ruleName,
											parameterNames,
											block.Get());
		}

		case TOKEN_ACTIONS: {
			// "actions" actionsFlags identifier [ "bind" list ]
			// "{" rawActions "}"
			PARSER_NONTERMINAL("actions definition");

			_NextToken();

			// actionsflags: ( actionsFlag | ("maxline" string) )*
			uint32_t actionsFlags = 0;
			while (_Token() == TOKEN_STRING) {
				ActionsFlagMap::const_iterator it =
					sActionsFlags.find(_Token());
				if (it == sActionsFlags.end())
					break;

				_NextToken();

				uint32_t flag = it->second;
				if (flag == data::RuleActions::MAX_LINE_FACTOR) {
					if (_Token() != TOKEN_STRING) {
						_ThrowExpected(
							"Expected number after 'actions' flag 'maxline'");
					}

					actionsFlags = (actionsFlags & data::RuleActions::FLAG_MASK)
						| (uint32_t)atoi(_Token().ToStlString().c_str())
							* data::RuleActions::MAX_LINE_FACTOR;
					_NextToken();
				} else
					actionsFlags |= flag;
			}

			// rule name
			if (_Token() != TOKEN_STRING)
				_ThrowExpected("Expected 'actions' rule name");

			data::String ruleName = _Token();
			_NextToken();

			// bind list
			code::NodeReference bindList;
			if (_Token() == TOKEN_BIND)
				bindList.SetTo(_ParseList(), true);

			// the actions block
			if (_Token() != TOKEN_LEFT_BRACE)
				_ThrowExpected("Expected '{' after 'actions' head");

			data::String commands(fLexer.ScanActions());
			_NextToken();

			_SkipToken(TOKEN_RIGHT_BRACE, "Expected '}' after 'actions' block");

			// create node
			return new code::ActionsDefinition(actionsFlags,
											   ruleName,
											   bindList.Get(),
											   commands);
		}

		default: {
			// 1. argument "on" list <assignmentOp> list ";"
			// 2. argument <assignmentOp> list ';'
			// 3. argument listOfLists ';'
			PARSER_NONTERMINAL("assigment/rule invocation");

			// each case starts with an argument
			code::NodeReference argument(_TryParseArgument(), true);
			if (argument.Get() == nullptr)
				return nullptr;

			switch (_Token().ID()) {
				case TOKEN_ON: {
					// 1. argument "on" list <assignmentOp> list ";"
					PARSER_NONTERMINAL("on assignment");

					_NextToken();
					code::NodeReference onTargets(_ParseList(), true);

					code::AssignmentOperator operatorType =
						_ParseAssignmentOperator();

					code::NodeReference rhs(_ParseList(), true);
					_SkipToken(
						TOKEN_SEMICOLON,
						"Expected ';' at the end of the assignment statement");

					// create node
					return new code::Assignment(argument.Get(),
												operatorType,
												rhs.Get(),
												onTargets.Get());
				}

				case TOKEN_ASSIGN:
				case TOKEN_ASSIGN_PLUS:
				case TOKEN_ASSIGN_DEFAULT: {
					// 2. argument <assignmentOp> list ';'
					PARSER_NONTERMINAL("assignment");

					code::AssignmentOperator operatorType =
						_ParseAssignmentOperator();

					code::NodeReference rhs(_ParseList(), true);
					_SkipToken(
						TOKEN_SEMICOLON,
						"Expected ';' at the end of the assignment statement");

					// create node
					return new code::Assignment(argument.Get(),
												operatorType,
												rhs.Get());
				}

				default: {
					// 3. argument listOfLists ';'
					PARSER_NONTERMINAL("rule invocation");

					NodeListContainer arguments;
					_ParseListOfLists(arguments);
					_SkipToken(
						TOKEN_SEMICOLON,
						"Expected ';' at the end of the rule invocation");

					// create node
					code::Node* result =
						new code::FunctionCall(argument.Get(),
											   arguments.fNodes);

					arguments.Detach();

					return result;
				}
			}
		}
	}
}

code::Node*
Parser::_ParseLocalVariableDeclaration()
{
	// "local" list [ "=" list ] ";"
	PARSER_NONTERMINAL("local variable");

	// "local" keyword
	_SkipToken(TOKEN_LOCAL, "Expected 'local' keyword");

	// variable list
	code::NodeReference variables(_ParseList(), true);

	// optional "=" initializer
	code::NodeReference initializer;
	if (_TrySkipToken(TOKEN_ASSIGN))
		initializer.SetTo(_ParseList(), true);

	// skip ";"
	_SkipToken(TOKEN_SEMICOLON,
			   "Expected ';' after local variable declaration");

	// create node
	return new code::LocalVariableDeclaration(variables.Get(),
											  initializer.Get());
}

code::List*
Parser::_ParseList()
{
	PARSER_NONTERMINAL("list");

	util::Reference<code::List> list(new code::List, true);

	// zero or more arguments
	while (code::Node* argument = _TryParseArgument(true))
		list.Get()->AppendKeepReference(argument);

	return list.Detach();
}

code::Node*
Parser::_TryParseArgument(bool allowKeyword)
{
	// either a bracket expression or a simple string
	PARSER_NONTERMINAL("argument");

	if (_TrySkipToken(TOKEN_LEFT_BRACKET)) {
		code::NodeReference result(_ParseBracketExpression(), true);
		_SkipToken(TOKEN_RIGHT_BRACKET,
				   "Expected ']' at the end of a bracket expression");
		return result.Detach();
	}

	if (_Token() == TOKEN_STRING || (allowKeyword && _Token().IsKeyword())) {
		code::Node* result = new code::Leaf(_Token());
		_NextToken();
		return result;
	}

	return nullptr;
}

code::Node*
Parser::_ParseBracketExpression()
{
	// either a bracketed "on" expression or a function call
	PARSER_NONTERMINAL("bracket expression");

	if (code::Node* node = _TryParseBracketOnExpression())
		return node;

	if (code::Node* node = _TryParseFunctionCall())
		return node;

	_ThrowExpected("Expected statement or local variable declaration");

	return nullptr;
}

code::Node*
Parser::_TryParseBracketOnExpression()
{
	// "on" keyword
	PARSER_NONTERMINAL("bracket on expression");

	if (!_TrySkipToken(TOKEN_ON))
		return nullptr;

	code::NodeReference onTarget(
		_Expect(_TryParseArgument(), "Expected target argument after 'on'"),
		true);

	// either a "return ..." or a function call
	code::NodeReference expression;
	if (_TrySkipToken(TOKEN_RETURN)) {
		expression.SetTo(_ParseList(), true);
	} else {
		expression.SetTo(_TryParseFunctionCall(), true);
		if (expression.Get() == nullptr) {
			_ThrowExpected(
				"Expected 'return' or function call in 'on' expression");
		}
	}

	// create result
	return new code::OnExpression(onTarget.Get(), expression.Get());
}

code::Node*
Parser::_TryParseFunctionCall()
{
	// the function name(s)
	PARSER_NONTERMINAL("function call expression");

	code::NodeReference function(_TryParseArgument(), true);
	if (function.Get() == nullptr)
		return nullptr;

	// list of lists of arguments
	NodeListContainer nodes;
	_ParseListOfLists(nodes);

	// create the node
	code::Node* result = new code::FunctionCall(function.Get(), nodes.fNodes);

	nodes.Detach();

	return result;
}

void
Parser::_ParseListOfLists(NodeListContainer& nodes)
{
	// one list or more lists, separated by ":"
	PARSER_NONTERMINAL("list of lists");

	nodes.Add(_ParseList());

	while (_TrySkipToken(TOKEN_COLON))
		nodes.Add(_ParseList());
}

code::Node*
Parser::_ParseExpression()
{
	// andExpression (["||" | "|"] andExpression)*
	PARSER_NONTERMINAL("expression");

	code::NodeReference result(_ParseAndExpression(), true);

	for (;;) {
		if (_Token().ID() != TOKEN_OR)
			return result.Detach();

		_NextToken();
		code::NodeReference rhs(_ParseAndExpression(), true);
		result.SetTo(new code::OrExpression(result.Get(), rhs.Get()), true);
	}
}

code::Node*
Parser::_ParseAndExpression()
{
	// comparison (["&&" | "&"] comparison)*
	PARSER_NONTERMINAL("andExpression");

	code::NodeReference result(_ParseComparison(), true);

	for (;;) {
		if (_Token().ID() != TOKEN_AND)
			return result.Detach();

		_NextToken();
		code::NodeReference rhs(_ParseComparison(), true);
		result.SetTo(new code::AndExpression(result.Get(), rhs.Get()), true);
	}
}

code::Node*
Parser::_ParseComparison()
{
	// atom (<op> atom)*
	PARSER_NONTERMINAL("comparison");

	code::NodeReference result(_ParseAtom(), true);

	for (;;) {
		switch (_Token().ID()) {
			case TOKEN_EQUAL: {
				_NextToken();
				code::NodeReference rhs(_ParseAtom(), true);
				result.SetTo(new code::EqualExpression(result.Get(), rhs.Get()),
							 true);
				break;
			}

			case TOKEN_NOT_EQUAL: {
				_NextToken();
				code::NodeReference rhs(_ParseAtom(), true);
				result.SetTo(
					new code::NotEqualExpression(result.Get(), rhs.Get()),
					true);
				break;
			}

			case TOKEN_LESS_OR_EQUAL: {
				_NextToken();
				code::NodeReference rhs(_ParseAtom(), true);
				result.SetTo(
					new code::LessOrEqualExpression(result.Get(), rhs.Get()),
					true);
				break;
			}

			case TOKEN_LESS: {
				_NextToken();
				code::NodeReference rhs(_ParseAtom(), true);
				result.SetTo(new code::LessExpression(result.Get(), rhs.Get()),
							 true);
				break;
			}

			case TOKEN_GREATER_OR_EQUAL: {
				_NextToken();
				code::NodeReference rhs(_ParseAtom(), true);
				result.SetTo(
					new code::GreaterOrEqualExpression(result.Get(), rhs.Get()),
					true);
				break;
			}

			case TOKEN_GREATER: {
				_NextToken();
				code::NodeReference rhs(_ParseAtom(), true);
				result.SetTo(
					new code::GreaterExpression(result.Get(), rhs.Get()),
					true);
				break;
			}

			default:
				return result.Detach();
		}
	}
}

code::Node*
Parser::_ParseAtom()
{
	PARSER_NONTERMINAL("atom");

	switch (_Token().ID()) {
		case TOKEN_NOT: {
			// "!" atom
			PARSER_NONTERMINAL("not expression");

			_NextToken();
			code::NodeReference atom(_ParseAtom(), true);

			// create node
			return new code::NotExpression(atom.Get());
		}

		case TOKEN_LEFT_PARENTHESIS: {
			// "(" expression ")"
			PARSER_NONTERMINAL("(expression)");

			_NextToken();
			code::NodeReference expression(_ParseExpression(), true);
			_SkipToken(TOKEN_RIGHT_PARENTHESIS,
					   "Expected ')' after expression");
			return expression.Detach();
		}

		default: {
			// argument [ "in" list ]
			PARSER_NONTERMINAL("argument (in list)");

			code::NodeReference argument(
				_Expect(_TryParseArgument(), "Expected argument"),
				true);

			// the "in" part is optional
			if (!_TrySkipToken(TOKEN_IN))
				return argument.Detach();

			code::NodeReference list(_ParseList(), true);

			// create node
			return new code::InListExpression(argument.Get(), list.Get());
		}
	}
}

code::AssignmentOperator
Parser::_ParseAssignmentOperator()
{
	switch (_Token().ID()) {
		case TOKEN_ASSIGN:
			_NextToken();
			return code::ASSIGNMENT_OPERATOR_ASSIGN;
		case TOKEN_ASSIGN_PLUS:
			_NextToken();
			return code::ASSIGNMENT_OPERATOR_APPEND;
		case TOKEN_ASSIGN_DEFAULT:
			_NextToken();
			return code::ASSIGNMENT_OPERATOR_DEFAULT;
		default:
			_ThrowExpected("Expected assignment operator");
			return code::ASSIGNMENT_OPERATOR_DEFAULT;
	}
}

void
Parser::_Throw(const char* message)
{
	throw ParseException(message, fFileName, fLexer.CurrentTokenPosition());
}

void
Parser::_ThrowExpected(const char* expected)
{
	std::string message(expected);
	if (_Token() == TOKEN_EOF)
		message += ". Encountered end of input";
	else
		message += ". Got '" + _Token().ToStlString() + "'";

	_Throw(message.c_str());
}

} // namespace parser
} // namespace ham
