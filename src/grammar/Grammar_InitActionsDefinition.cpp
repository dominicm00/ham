/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "grammar/Spirit.h"
	// precompiled -- must be first

#include "grammar/Grammar.h"

#include "code/ActionsDefinition.h"
#include "code/List.h"
#include "grammar/Iterator.h"
#include "grammar/Skipper.h"


namespace grammar {


namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;


template<>
void
Grammar<IteratorType, Skipper<IteratorType> >::_InitActionsDefinition()
{
	using ascii::char_;
	using phoenix::begin;
	using phoenix::construct;
	using phoenix::end;
	using phoenix::new_;
	using qi::_val;
	using qi::_1;
	using qi::_2;
	using qi::_3;
	using qi::_4;
	using qi::raw;

	fActionsDefinition
		= ("actions" >> fActionsFlags >> fIdentifier >> fActionsBindList
			>> '{'
			>> raw[fActions]
			>> '}')
					[ _val = new_<code::ActionsDefinition>(_1, _2, _3,
						construct<std::string>(begin(_4), end(_4))) ]
	;

	fActionsFlags
		= *(fActionFlag
				[ _val |= _1 ]
// TODO: Support "maxline ARG"!
			)
	;

	fActionsBindList
		= -("bind" >> fList)
	;

	fActions
		= *(+(char_ - '{' - '}')
			| '{' >> fActions >> '}'
			)
// TODO: This is a very crude approximation. Jam does a mite better, since it
// does at least recognize (full line) comments. This should be improved!
	;
}


} // namespace grammar
