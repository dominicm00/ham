/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_GRAMMAR_STRING_H
#define HAM_GRAMMAR_STRING_H


#include "grammar/Spirit.h"
	// precompiled -- must be first

#include "data/String.h"


namespace grammar {


namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;


template<typename Iterator>
struct String : qi::grammar<Iterator, data::String()> {
	String();

private:
	qi::rule<Iterator, data::String()> fString;
	qi::rule<Iterator, data::String()> fSubString;
	qi::rule<Iterator, char()> fQuotedChar;
	qi::rule<Iterator, char()> fUnquotedChar;
	qi::rule<Iterator, char()> fEscapedChar;
};


} // namespace grammar


#endif	// HAM_GRAMMAR_STRING_H
