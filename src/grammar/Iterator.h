/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_GRAMMAR_ITERATOR_H
#define HAM_GRAMMAR_ITERATOR_H


#include "grammar/Spirit.h"
	// precompiled -- must be first

#include <istream>


#include "grammar/Lexer.h"


namespace grammar {


	typedef std::istream_iterator<char> BaseIteratorType;
	typedef TokenIterator<BaseIteratorType> IteratorType;


} // namespace grammar


#endif	// HAM_GRAMMAR_ITERATOR_H
