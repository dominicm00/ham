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


template<typename Iterator, typename Skipper>
class Grammar : public qi::grammar<Iterator, code::Node*(), Skipper> {
public:
								Grammar(NodeRegistry& nodeRegistry);

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

	qi::rule<Iterator, code::Node*(), Skipper> fStart;
	qi::rule<Iterator, code::Block*(), Skipper> fBlock;
	qi::rule<Iterator, code::LocalVariableDeclaration*(), Skipper>
		fLocalVariableDeclaration;
	qi::rule<Iterator, code::RuleDefinition*(), Skipper> fRuleDefinition;
	qi::rule<Iterator, code::Node*(), Skipper> fActionsDefinition;
	qi::rule<Iterator, code::If*(), Skipper> fIfStatement;
	qi::rule<Iterator, code::Node*(), Skipper> fForStatement;
	qi::rule<Iterator, code::Node*(), Skipper> fWhileStatement;
	qi::rule<Iterator, code::Switch*(), Skipper> fSwitchStatement;
	qi::rule<Iterator, code::Case*(), Skipper> fCaseStatement;
	qi::rule<Iterator, code::Node*(), Skipper> fStatement;
	qi::rule<Iterator, code::FunctionCall*(), Skipper> fFunctionCall;
	qi::rule<Iterator, code::Node*(), Skipper> fOnStatement;
	qi::rule<Iterator, code::NodeList(), Skipper> fListOfLists;
	qi::rule<Iterator, code::List*(), Skipper> fList;
	qi::rule<Iterator, uint32_t(), Skipper> fActionsFlags;
	qi::rule<Iterator, code::List*(), Skipper> fActionsBindList;
	qi::rule<Iterator, qi::unused_type> fActions;
	qi::rule<Iterator, code::Node*(), Skipper> fAtom;
	qi::rule<Iterator, code::Node*(), Skipper> fExpression;
	qi::rule<Iterator, code::Node*(), Skipper> fArgument;
	qi::rule<Iterator, code::OnExpression*(), Skipper> fBracketOnExpression;
	qi::rule<Iterator, code::Node*(), Skipper> fBracketExpression;
	qi::rule<Iterator, data::StringBuffer()> fIdentifier;
	qi::rule<Iterator, data::StringBuffer()> fString;
	qi::rule<Iterator, data::StringBuffer()> fSubString;
	qi::rule<Iterator, char()> fQuotedChar;
	qi::rule<Iterator, char()> fUnquotedChar;
	qi::rule<Iterator, char()> fEscapedChar;

	qi::symbols<char, unsigned> fKeyword;
	qi::symbols<char, unsigned> fListDelimiter;
	qi::symbols<char, uint32_t> fActionFlag;
	qi::symbols<char, code::AssignmentOperator> fAssignmentOperator;
};


} // namespace grammar


#endif	// HAM_GRAMMAR_GRAMMAR_H
