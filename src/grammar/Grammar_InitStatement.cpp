/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "grammar/Grammar.h"

#include "code/ActionsDefinition.h"
#include "code/Assignment.h"
#include "code/Block.h"
#include "code/For.h"
#include "code/FunctionCall.h"
#include "code/If.h"
#include "code/Include.h"
#include "code/Jump.h"
#include "code/List.h"
#include "code/OnExpression.h"
#include "code/RuleDefinition.h"
#include "code/Switch.h"
#include "code/While.h"
#include "grammar/Iterator.h"
#include "grammar/Skipper.h"


namespace grammar {


namespace phoenix = boost::phoenix;


template<>
void
Grammar<IteratorType, Skipper<IteratorType> >::_InitStatement()
{
	using qi::_val;
	using qi::_1;
	using qi::_2;
	using qi::_3;
	using phoenix::new_;

	fStatement
		= ('{' >> fBlock >> '}')
				[ _val = _1 ]
		| ("include" >> fList >> ';')
				[ _val = new_<code::Include>(_1) ]
		| ("break" >> fList >> ';')
				[ _val = new_<code::Break>(_1) ]
		| ("continue" >> fList >> ';')
				[ _val = new_<code::Continue>(_1) ]
		| ("return" >> fList >> ';')
				[ _val = new_<code::Return>(_1) ]
		| ("jumptoeof" >> fList >> ';')
				[ _val = new_<code::JumpToEof>(_1) ]
		| fIfStatement
				[ _val = _1 ]
		| fForStatement
				[ _val = _1 ]
		| fWhileStatement
				[ _val = _1 ]
		| fSwitchStatement
				[ _val = _1 ]
		| fOnStatement
				[ _val = _1 ]
		| fRuleDefinition
				[ _val = _1 ]
		| fActionsDefinition
				[ _val = _1 ]
		| fArgument [_val = _1]
			>> (("on" >> fList >> fAssignmentOperator >> fList)
					[ _val = new_<code::Assignment>(_val, _2, _3, _1) ]
				| (fAssignmentOperator >> fList)
					[ _val = new_<code::Assignment>(_val, _1, _2) ]
				| fListOfLists
					[ _val = new_<code::FunctionCall>(_val, _1) ]
			) >> ';'
	;
}


} // namespace grammar
