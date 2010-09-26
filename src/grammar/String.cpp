/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "grammar/Spirit.h"
	// precompiled -- must be first

#include "grammar/String.h"

#include "grammar/Iterator.h"
#include "grammar/Symbols.h"


namespace grammar {


template<>
grammar::String<IteratorType>::String()
	:
	String::base_type(fString),
	fString(std::string("string")),
	fSubString(std::string("subString")),
	fQuotedChar(std::string("quotedChar")),
	fUnquotedChar(std::string("unquotedChar")),
	fEscapedChar(std::string("escapedChar"))
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
		= !(kListDelimiter >> (space | eoi))
			>> +fSubString
				[ _val += _1 ]
	;

#if 0
	qi::debug(fString);
	qi::debug(fSubString);
#endif
}


} // namespace grammar
