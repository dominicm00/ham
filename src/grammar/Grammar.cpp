/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "grammar/Spirit.h"
	// precompiled -- must be first

#include "grammar/Grammar.h"

#include "code/Assignment.h"
#include "code/Block.h"
#include "code/Case.h"
#include "code/For.h"
#include "code/FunctionCall.h"
#include "code/If.h"
#include "code/Include.h"
#include "code/InListExpression.h"
#include "code/Jump.h"
#include "code/List.h"
#include "code/LocalVariableDeclaration.h"
#include "code/OnExpression.h"
#include "code/RuleDefinition.h"
#include "code/Switch.h"
#include "code/While.h"
#include "grammar/ActorFactory.h"
#include "grammar/Iterator.h"
#include "grammar/Skipper.h"


namespace grammar {


namespace phoenix = boost::phoenix;
namespace ascii = boost::spirit::ascii;


template<>
Grammar<IteratorType>::Grammar(
	LexerType& lexer, NodeRegistry& nodeRegistry)
	:
	Grammar::base_type(fStart),
	fNodeRegistry(nodeRegistry),
	fFactory(fNodeRegistry),
	fStart(std::string("start")),
	fBlock(std::string("block")),
	fLocalVariableDeclaration(std::string("localVariableDeclaration")),
	fRuleDefinition(std::string("ruleDefinition")),
	fActionsDefinition(std::string("actionsDefinition")),
	fIfStatement(std::string("ifStatement")),
	fForStatement(std::string("forStatement")),
	fWhileStatement(std::string("whileStatement")),
	fSwitchStatement(std::string("switchStatement")),
	fCaseStatement(std::string("caseStatement")),
	fStatement(std::string("statement")),
	fFunctionCall(std::string("functionCall")),
	fOnStatement(std::string("onStatement")),
	fListOfLists(std::string("listOfLists")),
	fList(std::string("list")),
	fActionsFlags(std::string("actionsFlags")),
	fActionsBindList(std::string("actionsBindList")),
	fActions(std::string("actions")),
	fAtom(std::string("atom")),
	fExpression(std::string("expression")),
	fArgument(std::string("argument")),
	fBracketOnExpression(std::string("bracketOnExpression")),
	fBracketExpression(std::string("bracketExpression")),
	fIdentifier(std::string("identifier")),
	fString(std::string("string"))
{
	using qi::eps;
	using qi::_val;
	using qi::_1;
	using qi::_2;
	using qi::_3;
	using qi::_4;
	using qi::_5;
	using qi::_6;
	using qi::token;

	ActorFactory<Factory> factory(fFactory);

	fAssignmentOperator.add
		("=", code::ASSIGNMENT_OPERATOR_ASSIGN)
		("+=", code::ASSIGNMENT_OPERATOR_APPEND)
		("?=", code::ASSIGNMENT_OPERATOR_DEFAULT)
	;

	fActionFlag.add
		("updated",		kActionFlagUpdated)
		("together",	kActionFlagTogether)
		("ignore",		kActionFlagIgnore)
		("quietly",		kActionFlagQuietly)
		("piecemeal",	kActionFlagPiecemeal)
		("existing",	kActionFlagExisting)
	;

	_InitActionsDefinition();
	_InitArgument();
	_InitExpression();
	_InitRuleDefinition();
	_InitStatement();
	_InitString();

	fList = eps
			[ _val = factory.Create<code::List>() ]
		>> *(fArgument
				[ *_val += _1 ]
			);

	fListOfLists = fList % token(TOKEN_COLON);

	fFunctionCall
		= (fArgument >> fListOfLists)
				[ _val = factory.Create<code::FunctionCall>(_1, _2) ]
	;

	fBlock
		= eps
				[ _val = factory.Create<code::Block>() ]
			>> *(fStatement
					[ *_val += _1 ]
				| fLocalVariableDeclaration
					[ *_val += _1 ]
				)
	;

	fLocalVariableDeclaration
		= token(TOKEN_LOCAL) >> fList
				[ _val = factory.Create<code::LocalVariableDeclaration>(_2) ]
			>> -(token(TOKEN_ASSIGN) >> fList
					[ bind(&code::LocalVariableDeclaration::SetInitializer,
						_val, _2) ]
				)
			>> token(TOKEN_SEMICOLON)
	;

	fIfStatement
		= (token(TOKEN_IF) >> fExpression >> token(TOKEN_LEFT_BRACE)
			>> fBlock >> token(TOKEN_RIGHT_BRACE))
					[ _val = factory.Create<code::If>(_2, _4) ]
			>> -(token(TOKEN_ELSE) >> fStatement
					[ bind(&code::If::SetElseBlock, _val, _2) ]
				)
	;

	fForStatement
		= (token(TOKEN_FOR) >> fArgument >> token(TOKEN_IN) >> fList
			>> token(TOKEN_LEFT_BRACE) >> fBlock >> token(TOKEN_RIGHT_BRACE))
					[ _val = factory.Create<code::For>(_2, _4, _6) ]
	;

	fWhileStatement
		= (token(TOKEN_WHILE) >> fExpression >> token(TOKEN_LEFT_BRACE)
			>> fBlock >> token(TOKEN_RIGHT_BRACE))
					[ _val = factory.Create<code::While>(_2, _4) ]
	;

	fOnStatement
		= (token(TOKEN_ON) >> fArgument >> fStatement)
				[ _val = factory.Create<code::OnExpression>(_2, _3) ]
	;

	fCaseStatement
		= (token(TOKEN_CASE) >> fIdentifier >> token(TOKEN_COLON) >> fBlock)
				[ _val = factory.Create<code::Case>(_2, _4) ]
	;

	fSwitchStatement
		= (token(TOKEN_SWITCH) >> fList)
				[ _val = factory.Create<code::Switch>(_2) ]
			>> token(TOKEN_LEFT_BRACE)
			>> *(fCaseStatement
					[ bind(&code::Switch::AddCase, _val, _1) ]
				)
			>> token(TOKEN_RIGHT_BRACE)
	;

	fStart = fBlock;

#if 0
	qi::debug(fStart);
	qi::debug(fBlock);
	qi::debug(fLocalVariableDeclaration);
	qi::debug(fRuleDefinition);
	qi::debug(fActionsDefinition);
	qi::debug(fIfStatement);
	qi::debug(fForStatement);
	qi::debug(fWhileStatement);
	qi::debug(fSwitchStatement);
	qi::debug(fCaseStatement);
	qi::debug(fStatement);
	qi::debug(fFunctionCall);
	qi::debug(fOnStatement);
	qi::debug(fListOfLists);
	qi::debug(fList);
	qi::debug(fActionsFlags);
	qi::debug(fActionsBindList);
	qi::debug(fActions);
	qi::debug(fAtom);
	qi::debug(fExpression);
	qi::debug(fBracketExpression);
	qi::debug(fBracketOnExpression);
	qi::debug(fArgument);
	qi::debug(fIdentifier);
	qi::debug(fString);
	qi::debug(fSubString);
#endif
}


} // namespace grammar
