/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_COMPATIBILITY_H
#define HAM_COMPATIBILITY_H


namespace ham {


enum Compatibility {
	COMPATIBILITY_JAM,			// plain jam 2.5
	COMPATIBILITY_BOOST_JAM,	// Boost.Jam (version?)
	COMPATIBILITY_HAM_1,		// Ham 1.0

	COMPATIBILITY_HAM = COMPATIBILITY_HAM_1,

	COMPATIBILITY_MASK_ALL = (1 << COMPATIBILITY_JAM)
		| (1 << COMPATIBILITY_BOOST_JAM)
		| (1 << COMPATIBILITY_HAM_1)
};


}	// namespace ham


#endif	// HAM_COMPATIBILITY_H
