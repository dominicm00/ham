/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_GRAMMAR_GRAMMAR_H
#define HAM_GRAMMAR_GRAMMAR_H


#include "grammar/Spirit.h"
	// precompiled -- must be first

#include <list>

#include "code/Assignment.h"
	// TODO: Remove/move, if possible!
#include "code/Node.h"
#include "grammar/Lexer.h"
#include "grammar/NodeRegistry.h"
#include "grammar/SimpleFactory.h"


namespace code {
	class Block;
	class Case;
	class FunctionCall;
	class If;
	class List;
	class LocalVariableDeclaration;
	class OnExpression;
	class RuleDefinition;
	class Switch;
}


// TODO: Move to code::.
static const uint32_t kActionFlagUpdated		= 0x01;
static const uint32_t kActionFlagTogether		= 0x02;
static const uint32_t kActionFlagIgnore			= 0x04;
static const uint32_t kActionFlagQuietly		= 0x08;
static const uint32_t kActionFlagPiecemeal		= 0x10;
static const uint32_t kActionFlagExisting		= 0x20;
static const uint32_t kActionFlagMaxLineFactor	= 0x40;


namespace boost { namespace spirit { namespace traits {

template<>
struct container_value<data::StringBuffer>
{
    typedef data::String	type;
};

}}}


namespace grammar {


namespace qi = boost::spirit::qi;


template<typename Iterator>
class Grammar
	: public qi::grammar<Iterator, code::Node*()> {
public:
			typedef typename Iterator::LexerType LexerType;

public:
								Grammar(LexerType& lexer,
									NodeRegistry& nodeRegistry);

private:
			typedef SimpleFactory<NodeRegistry::RegistryHook> Factory;

private:
			void				_InitActionsDefinition();
			void				_InitArgument();
			void				_InitExpression();
			void				_InitRuleDefinition();
			void				_InitStatement();
			void				_InitString();

private:
			NodeRegistry&		fNodeRegistry;
			Factory				fFactory;

	qi::rule<Iterator, code::Node*()> fStart;
	qi::rule<Iterator, code::Block*()> fBlock;
	qi::rule<Iterator, code::LocalVariableDeclaration*()>
		fLocalVariableDeclaration;
	qi::rule<Iterator, code::RuleDefinition*()> fRuleDefinition;
	qi::rule<Iterator, code::Node*()> fActionsDefinition;
	qi::rule<Iterator, code::If*()> fIfStatement;
	qi::rule<Iterator, code::Node*()> fForStatement;
	qi::rule<Iterator, code::Node*()> fWhileStatement;
	qi::rule<Iterator, code::Switch*()> fSwitchStatement;
	qi::rule<Iterator, code::Case*()> fCaseStatement;
	qi::rule<Iterator, code::Node*()> fStatement;
	qi::rule<Iterator, code::FunctionCall*()> fFunctionCall;
	qi::rule<Iterator, code::Node*()> fOnStatement;
	qi::rule<Iterator, code::NodeList()> fListOfLists;
	qi::rule<Iterator, code::List*()> fList;
	qi::rule<Iterator, uint32_t()> fActionsFlags;
	qi::rule<Iterator, code::List*()> fActionsBindList;
	qi::rule<Iterator, qi::unused_type> fActions;
	qi::rule<Iterator, code::Node*()> fAtom;
	qi::rule<Iterator, code::Node*()> fExpression;
	qi::rule<Iterator, code::Node*()> fArgument;
	qi::rule<Iterator, code::OnExpression*()> fBracketOnExpression;
	qi::rule<Iterator, code::Node*()> fBracketExpression;
	qi::rule<Iterator, data::String()> fIdentifier;
	qi::rule<Iterator, data::String()> fString;

	qi::symbols<char, uint32_t> fActionFlag;
	qi::symbols<char, code::AssignmentOperator> fAssignmentOperator;
};


} // namespace grammar


#endif	// HAM_GRAMMAR_GRAMMAR_H
