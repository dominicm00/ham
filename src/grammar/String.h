/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_GRAMMAR_STRING_H
#define HAM_GRAMMAR_STRING_H


#include <boost/spirit/include/qi.hpp>

#include "data/String.h"


namespace grammar {


namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;


template <typename Iterator>
struct String : qi::grammar<Iterator, data::String()> {
	String()
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

// TODO: This shouldn't be defined here!
		fListDelimiter.add
			(":", 0)
			(";", 1)
			("]", 2)
			("=", 3)
			("+=", 3)
			("?=", 3)
			("||", 3)
			("|", 3)
			("&&", 3)
			("&", 3)
			("!=", 3)
			(")", 3)
			("<", 3)
			("<=", 3)
			(">", 3)
			(">=", 3)
			("{", 3)
			("}", 3)
		;

		fQuotedChar = fEscapedChar | (char_ - '"');
		fUnquotedChar = fEscapedChar | (char_ - '"' - space);
		fEscapedChar = '\\' >> char_;

		fSubString = ('"' >> *fQuotedChar >> '"') | +fUnquotedChar;
		fString
			= !(fListDelimiter >> (space | eoi))
				>> +fSubString
					[ _val += _1 ]
		;

#if 1
		qi::debug(fString);
		qi::debug(fSubString);
#endif
	}

private:
	qi::rule<Iterator, data::String()> fString;
	qi::rule<Iterator, data::String()> fSubString;
	qi::rule<Iterator, char()> fQuotedChar;
	qi::rule<Iterator, char()> fUnquotedChar;
	qi::rule<Iterator, char()> fEscapedChar;
	qi::symbols<char, unsigned> fListDelimiter;
};


} // namespace grammar


#endif	// HAM_GRAMMAR_STRING_H
