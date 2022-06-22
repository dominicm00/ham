/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_BEHAVIOR_BEHAVIOR_H
#define HAM_BEHAVIOR_BEHAVIOR_H

#include "behavior/Compatibility.h"

namespace ham
{
namespace behavior
{

/**
 * Encapulates language differences between Ham and other Jam interpreters. This
 * is separate from ruleset compatability, which changes the default Ham rules
 * available.
 *
 * Behaviors can include bugs, unintuitive behaviors, or changes to built-in
 * rules (ones defined directly by the interpreter).
 */
class Behavior
{
  public:
	enum EchoTrailingSpace {
		/**
		 * Append a space at the end of an Echo'd string.
		 */
		ECHO_TRAILING_SPACE,

		/**
		 * Echo strings as-is, without appending a space.
		 */
		ECHO_NO_TRAILING_SPACE
	};

	enum PathRootReplacerSlash {
		/**
		 * Always add a trailing slash when replacing a path's root component.
		 * This causes duplicate slashes if the path has a trailing slash.

		 \verbatim
		 my_program = bash ;
		 Echo $(my_program:R=/bin/) ;

		 > /bin//bash
		 \endverbatim
		 */
		PATH_ROOT_REPLACER_SLASH_ALWAYS,

		/**
		 * Only add a trailing slash when one does not already exist. Avoids
		 * duplicate slashes.

		 \verbatim
		 my_program = bash ;
		 Echo $(my_program:R=/bin/) ;

		 > /bin/bash
		 \endverbatim
		 */
		PATH_ROOT_REPLACER_SLASH_AVOID_DUPLICATE
	};

	enum BrokenSubscriptJoin {
		/**
		 * Return an empty list when a join operation has an end subscript less
		 * than the list length. Emulates the broken behavior of Jam.

		 \verbatim
		 my_list = Ham is super fun ;
		 Echo $(my_list[1-3]:J=-) ;
		 Echo $(my_list[1-4]:J=-) ;

		 >
		 > Ham-is-super-fun
		 \endverbatim
		 */
		BROKEN_SUBSCRIPT_JOIN,

		/**
		 * Correctly handle joins with subscripts.

		 \verbatim
		 my_list = Ham is super fun ;
		 Echo $(my_list[1-3]:J=-) ;
		 Echo $(my_list[1-4]:J=-) ;

		 > Ham-is-super
		 > Ham-is-super-fun
		 \endverbatim
		 */
		NO_BROKEN_SUBSCRIPT_JOIN
	};

	enum JoinCaseOperator {
		/**
		 * Execute joins before to-upper or to-lower operators. The join
		 * parameter will be affected by the case operator.

		 \verbatim
		 my_list = Ham is super fun ;
		 Echo $(my_list:LJ=X) ;

		 > hamxisxsuperxfun
		 \endverbatim
		 */
		JOIN_BEFORE_CASE_OPERATOR,

		/**
		 * Execute joins after to-upper or to-lower operators. The join
		 * parameter will not be affected by the case operator.

		 \verbatim
		 my_list = Ham is super fun ;
		 Echo $(my_list:LJ=X) ;

		 > hamXisXsuperXfun
		 \endverbatim
		 */
		JOIN_AFTER_CASE_OPERATOR
	};

  public:
	Behavior(Compatibility compatibility);

	EchoTrailingSpace GetEchoTrailingSpace() const
	{
		return fEchoTrailingSpace;
	}
	PathRootReplacerSlash GetPathRootReplacerSlash() const
	{
		return fPathRootReplacerSlash;
	}
	BrokenSubscriptJoin GetBrokenSubscriptJoin() const
	{
		return fBrokenSubscriptJoin;
	}
	JoinCaseOperator GetJoinCaseOperator() const { return fJoinCaseOperator; }

  private:
	EchoTrailingSpace fEchoTrailingSpace;
	PathRootReplacerSlash fPathRootReplacerSlash;
	BrokenSubscriptJoin fBrokenSubscriptJoin;
	JoinCaseOperator fJoinCaseOperator;
};

} // namespace behavior
} // namespace ham

#endif // HAM_BEHAVIOR_BEHAVIOR_H
