/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_BEHAVIOR_COMPATIBILITY_HPP
#define HAM_BEHAVIOR_COMPATIBILITY_HPP

namespace ham::behavior
{

/**
 * Available compatability modes. Includes other intepreters and
 * current/previous Ham versions.
 */
enum Compatibility {
	COMPATIBILITY_JAM,		 ///< Perforce Jam 2.5
	COMPATIBILITY_BOOST_JAM, ///< Boost.Jam
	COMPATIBILITY_HAM_1,	 ///< Ham 1.0

	COMPATIBILITY_HAM = COMPATIBILITY_HAM_1
	///< Current Ham version
};

enum {
	COMPATIBILITY_MASK_ALL = (1 << COMPATIBILITY_JAM)
		| (1 << COMPATIBILITY_BOOST_JAM) | (1 << COMPATIBILITY_HAM_1)
};

} // namespace ham::behavior

#endif // HAM_BEHAVIOR_COMPATIBILITY_HPP
