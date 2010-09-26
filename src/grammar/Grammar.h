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
#include "grammar/Expression.h"


namespace code {
	class ActionsDefinition;
	class Block;
	class Case;
	class For;
	class FunctionCall;
	class If;
	class List;
	class LocalVariableDeclaration;
	class OnExpression;
	class Node;
	class RuleDefinition;
	class Switch;
	class While;

	typedef std::list<List*> ListList;
		// TODO: Not nice!
}


namespace grammar {


namespace qi = boost::spirit::qi;


template<typename Iterator, typename Skipper>
class Grammar : public qi::grammar<Iterator, code::Node*(), Skipper> {
public:
								Grammar();

private:
			void				_InitActionsDefinition();
			void				_InitArgument();
			void				_InitExpression();
			void				_InitRuleDefinition();
			void				_InitStatement();

private:
	qi::rule<Iterator, code::Node*(), Skipper> fStart;
	qi::rule<Iterator, code::Block*(), Skipper> fBlock;
	qi::rule<Iterator, code::LocalVariableDeclaration*(), Skipper>
		fLocalVariableDeclaration;
	qi::rule<Iterator, code::RuleDefinition*(), Skipper> fRuleDefinition;
	qi::rule<Iterator, code::ActionsDefinition*(), Skipper> fActionsDefinition;
	qi::rule<Iterator, code::If*(), Skipper> fIfStatement;
	qi::rule<Iterator, code::For*(), Skipper> fForStatement;
	qi::rule<Iterator, code::While*(), Skipper> fWhileStatement;
	qi::rule<Iterator, code::Switch*(), Skipper> fSwitchStatement;
	qi::rule<Iterator, code::Case*(), Skipper> fCaseStatement;
	qi::rule<Iterator, code::Node*(), Skipper> fStatement;
	qi::rule<Iterator, code::FunctionCall*(), Skipper> fFunctionCall;
	qi::rule<Iterator, code::OnExpression*(), Skipper> fOnStatement;
	qi::rule<Iterator, code::ListList(), Skipper> fListOfLists;
	qi::rule<Iterator, code::List*(), Skipper> fList;
	qi::rule<Iterator, uint32_t(), Skipper> fActionsFlags;
	qi::rule<Iterator, code::List*(), Skipper> fActionsBindList;
	qi::rule<Iterator, qi::unused_type> fActions;
	qi::rule<Iterator, code::Node*(), Skipper> fAtom;
	qi::rule<Iterator, code::Node*(), Skipper> fExpression;
	qi::rule<Iterator, code::Node*(), Skipper> fArgument;
	qi::rule<Iterator, code::OnExpression*(), Skipper> fBracketOnExpression;
	qi::rule<Iterator, code::Node*(), Skipper> fBracketExpression;
	qi::rule<Iterator, data::String()> fIdentifier;

	grammar::String<Iterator> fString;

	qi::symbols<char, uint32_t> fActionFlag;
	qi::symbols<char, code::AssignmentOperator> fAssignmentOperator;
};


} // namespace grammar


#endif	// HAM_GRAMMAR_GRAMMAR_H
