/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "grammar/Spirit.h"
	// precompiled -- must be first

#include "grammar/Symbols.h"


namespace grammar {


boost::spirit::qi::symbols<char, unsigned> kListDelimiter;


static struct _StaticInitializer {
	_StaticInitializer()
	{
		kListDelimiter =
			":",
			";",
			"]",
			"=",
			"+=",
			"?=",
			"||",
			"|",
			"&&",
			"&",
			"!=",
			")",
			"<",
			"<=",
			">",
			">=",
			"{",
			"}"
		;
	}
} _initializer;


} // namespace grammar
