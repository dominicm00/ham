/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "grammar/Spirit.h"
	// precompiled -- must be first

#include "grammar/Grammar.h"

#include "code/FunctionCall.h"
#include "code/Leaf.h"
#include "code/List.h"
#include "code/OnExpression.h"
#include "grammar/ActorFactory.h"
#include "grammar/Iterator.h"
#include "grammar/Skipper.h"


namespace grammar {


namespace phoenix = boost::phoenix;


template<>
void
Grammar<IteratorType, Skipper<IteratorType> >::_InitArgument()
{
	using qi::_val;
	using qi::_1;

	ActorFactory<Factory> factory(fFactory);

	fArgument
		= ("[" >> fBracketExpression >> "]")
				[ _val = _1 ]
		| fString
				[ _val = factory.Create<code::Leaf>(_1) ];


	fBracketOnExpression
		= "on" >> fArgument
				[ _val = factory.Create<code::OnExpression>(_1) ]
			>> (
				("return" >> fList)
					[ bind(&code::OnExpression::SetExpression, _val, _1) ]
			|	fFunctionCall
					[ bind(&code::OnExpression::SetExpression, _val, _1) ]
			)
	;
		// Unfortunately [ _val = factory.Create<code::OnExpression>(_1, _2) ]
		// doesn't work here, since the variant isn't converted automatically to
		// Node*.

	fBracketExpression
		= fBracketOnExpression
		| fFunctionCall
	;
}


} // namespace grammar
