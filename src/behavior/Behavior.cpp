/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Copyright 2022, Dominic Martinez, dom@dominicm.dev.
 * Distributed under the terms of the MIT License.
 */

#include "behavior/Behavior.hpp"

namespace ham::behavior
{

/**
 * Given a compatability mode, sets the appropriate behaviors.
 *
 * \param[in] compatibility The tool/version to emulate.
 */
Behavior::Behavior(Compatibility compatibility)
	: fEchoTrailingSpace(ECHO_NO_TRAILING_SPACE),
	  fPathRootReplacerSlash(PATH_ROOT_REPLACER_SLASH_AVOID_DUPLICATE),
	  fBrokenSubscriptJoin(NO_BROKEN_SUBSCRIPT_JOIN),
	  fJoinCaseOperator(JOIN_AFTER_CASE_OPERATOR),
	  fErrorUpdatedSource(ERROR_INDEPENDENT_UPDATED)
{
	switch (compatibility) {
		case COMPATIBILITY_JAM:
			fEchoTrailingSpace = ECHO_TRAILING_SPACE;
			fPathRootReplacerSlash = PATH_ROOT_REPLACER_SLASH_ALWAYS;
			fBrokenSubscriptJoin = BROKEN_SUBSCRIPT_JOIN;
			fErrorUpdatedSource = WARN_INDEPENDENT_UPDATED;
			break;
		case COMPATIBILITY_BOOST_JAM:
			fBrokenSubscriptJoin = BROKEN_SUBSCRIPT_JOIN;
			fJoinCaseOperator = JOIN_BEFORE_CASE_OPERATOR;
			fErrorUpdatedSource = WARN_INDEPENDENT_UPDATED;
			break;
		case COMPATIBILITY_HAM_1:
			break;
	}
}

} // namespace ham::behavior
