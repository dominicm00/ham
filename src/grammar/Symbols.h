/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_GRAMMAR_SYMBOLS_H
#define HAM_GRAMMAR_SYMBOLS_H


#include <boost/spirit/include/qi.hpp>


namespace grammar {


extern boost::spirit::qi::symbols<char, unsigned> kListDelimiter;


} // namespace grammar


#endif	// HAM_GRAMMAR_SYMBOLS_H
