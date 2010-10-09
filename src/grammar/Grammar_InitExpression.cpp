/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "grammar/Spirit.h"
	// precompiled -- must be first

#include "grammar/Grammar.h"

#include "code/BinaryExpression.h"
#include "code/InListExpression.h"
#include "code/List.h"
#include "code/NotExpression.h"
#include "grammar/ActorFactory.h"
#include "grammar/Iterator.h"
#include "grammar/Skipper.h"


namespace grammar {


namespace phoenix = boost::phoenix;


template<>
void
Grammar<IteratorType, Skipper<IteratorType> >::_InitExpression()
{
	using qi::_val;
	using qi::_1;

	ActorFactory<Factory> factory(fFactory);

	fAtom
		= '!' >> fAtom
				[ _val = factory.Create<code::NotExpression>(_1) ]
		| ('(' >> fExpression >> ')')
				[ _val = _1 ]
		| fArgument
				[ _val = _1 ]
			>> -("in" >> fList)
				[ _val = factory.Create<code::InListExpression>(_val, _1) ]
	;

	fExpression
		= fAtom
				[ _val = _1 ]
			>> *(
				'=' >> fAtom
					[ _val = factory.Create<code::EqualExpression>(_val, _1) ]
			|	"!=" >> fAtom
					[ _val = factory.Create<code::NotEqualExpression>(_val, _1) ]
			|	"<=" >> fAtom
					[ _val = factory.Create<code::LessOrEqualExpression>(_val,
						_1) ]
			|	'<' >> fAtom
					[ _val = factory.Create<code::LessExpression>(_val, _1) ]
			|	">=" >> fAtom
					[ _val = factory.Create<code::GreaterOrEqualExpression>(
						_val, _1) ]
			|	'>' >> fAtom
					[ _val = factory.Create<code::GreaterExpression>(_val, _1) ]
			|	"&&" >> fAtom
					[ _val = factory.Create<code::AndExpression>(_val, _1) ]
			|	'&' >> fAtom
					[ _val = factory.Create<code::AndExpression>(_val, _1) ]
			|	"||" >> fAtom
					[ _val = factory.Create<code::OrExpression>(_val, _1) ]
			|	"|" >> fAtom
					[ _val = factory.Create<code::OrExpression>(_val, _1) ]
			)
	;
}


} // namespace grammar
