/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "grammar/Spirit.h"
	// precompiled -- must be first

#include "grammar/Grammar.h"

#include "grammar/Iterator.h"
#include "grammar/Skipper.h"


namespace grammar {


namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;


template<>
void
Grammar<IteratorType, Skipper<IteratorType> >::_InitString()
{
	using qi::eoi;
	using qi::eps;
	using qi::space;
	using qi::_val;
	using qi::_1;
	using ascii::char_;

	fQuotedChar = fEscapedChar | (char_ - '"');
	fUnquotedChar = fEscapedChar | (char_ - '"' - space);
	fEscapedChar = '\\' >> char_;

	fSubString = ('"' >> *fQuotedChar >> '"') | +fUnquotedChar;
	fString
		= !(fListDelimiter >> (space | eoi))
			>> +fSubString
				[ _val += _1 ]
	;

	fIdentifier
		= !((fListDelimiter | fKeyword) >> (space | eoi))
			>> +fSubString
				[ _val += _1 ]
	;
}


} // namespace grammar
