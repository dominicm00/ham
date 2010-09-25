/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_GRAMMAR_STRING_H
#define HAM_GRAMMAR_STRING_H


#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;


namespace grammar {


template <typename Iterator>
struct Skipper : qi::grammar<Iterator> {
	Skipper() : Skipper::base_type(fStart)
	{
		using qi::char_;
		using qi::lit;
		using qi::space;

		fStart = space | lit('#') >> *(char_ - '\n' - '\r');
	}

private:
	qi::rule<Iterator> fStart;
};


} // namespace grammar


#endif	// HAM_GRAMMAR_STRING_H
