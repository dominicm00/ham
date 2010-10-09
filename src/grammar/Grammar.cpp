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
Grammar<IteratorType, Skipper<IteratorType> >::Grammar(
	NodeRegistry& nodeRegistry)
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
	fString(std::string("string")),
	fSubString(std::string("subString")),
	fQuotedChar(std::string("quotedChar")),
	fUnquotedChar(std::string("unquotedChar")),
	fEscapedChar(std::string("escapedChar"))
{
	using qi::eps;
	using qi::_val;
	using qi::_1;
	using qi::_2;
	using qi::_3;
	using qi::_4;

	ActorFactory<Factory> factory(fFactory);

	fKeyword =
		"local",
		"include",
		"jumptoeof",
		"on",
		"break",
		"continue",
		"return",
		"for",
		"in",
		"switch",
		"if",
		"else",
		"while",
		"rule",
		"actions"
	;

	fListDelimiter =
		":",
		";",
		"]",
		"=",
		"+=",
		"?=",
		"||",
		"|",
		"&&",
		"&",
		"!=",
		")",
		"<",
		"<=",
		">",
		">=",
		"{",
		"}"
	;

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

	fListOfLists = fList % ':';

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
		= "local" >> fList
				[ _val = factory.Create<code::LocalVariableDeclaration>(_1) ]
			>> -('=' >> fList
					[ bind(&code::LocalVariableDeclaration::SetInitializer,
						_val, _1) ]
				)
			>> ';'
	;

	fIfStatement
		= ("if" >> fExpression >> '{' >> fBlock >> '}')
				[ _val = factory.Create<code::If>(_1, _2) ]
			>> -("else" >> fStatement
					[ bind(&code::If::SetElseBlock, _val, _1) ]
				)
	;

	fForStatement
		= ("for" >> fArgument >> "in" >> fList >> '{' >> fBlock >> '}')
				[ _val = factory.Create<code::For>(_1, _2, _3) ]
	;

	fWhileStatement
		= ("while" >> fExpression >> '{' >> fBlock >> '}')
				[ _val = factory.Create<code::While>(_1, _2) ]
	;

	fOnStatement
		= ("on" >> fArgument >> fStatement)
				[ _val = factory.Create<code::OnExpression>(_1, _2) ]
	;

	fCaseStatement
		= ("case" >> fIdentifier >> ':' >> fBlock)
				[ _val = factory.Create<code::Case>(_1, _2) ]
	;

	fSwitchStatement
		= ("switch" >> fList)
				[ _val = factory.Create<code::Switch>(_1) ]
			>> '{'
			>> *(fCaseStatement
					[ bind(&code::Switch::AddCase, _val, _1) ]
				)
			>> '}'
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
