/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_BEHAVIOR_BEHAVIOR_HPP
#define HAM_BEHAVIOR_BEHAVIOR_HPP

#include "behavior/Compatibility.hpp"

namespace ham::behavior
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
		 -
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
		 -
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
		 -
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
		 -
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
		 -
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
		 -
		 > hamXisXsuperXfun
		 \endverbatim
		 */
		JOIN_AFTER_CASE_OPERATOR
	};

	/**
	 * Independent targets occur when actions are invoked on a target/source
	 * pair that do not have a dependency.

	 \verbatim
	 actions SayHi
	 {
		echo "Hi from $(1) to $(2) "
	 }

	 SayHi target : source1 source2 ;
	 Depends target : source1 ;
	 Depends all : target ;
	 \endverbatim

	 * In the above example, source2 is an independent target because it is
	 * invoked as a source for target, but a Depends rule was never called.
	 *
	 * This behavior controls what dependency relationship is formed for
	 * independent targets.
	 */
	enum IndependentTarget {
		/**
		 * Create implicit dependencies for independent targets.

		 \verbatim
		 actions SayHi
		 {
			echo "Hi from $(1) to $(2) "
		 }

		 NOTFILE source2 other-source ;
		 SayHi target : source1 source2 ;
		 SayHi source1 : other-source ;
		 Depends all : target ;
		 -
		 > Hi from source1 to other-source
		 > Hi from target to source1 source2
		 \endverbatim
		 */
		CREATE_IMPLICIT_DEPENDENCY,

		/**
		 * Don't create implicit dependencies for independent targets.

		 \verbatim
		 actions SayHi
		 {
			echo "Hi from $(1) to $(2) "
		 }

		 NOTFILE source2 other-source ;
		 SayHi target : source1 source2 ;
		 SayHi source1 : other-source ;
		 Depends all : target ;
		 -
		 > Hi from target to source1 source2
		 \endverbatim
		 */
		NO_CREATE_IMPLICIT_DEPENDENCY
	};

	/**
	 * Error strictness for independent targets; see Behavior::IndependentTarget
	 * for more info.
	 */
	enum IndependentTargetStrictness {
		/**
		 * Allow independent targets without any output.
		 */
		ALLOW_INDEPENDENT_TARGET,

		/**
		 * Print warning for independent targets.
		 */
		WARN_INDEPENDENT_TARGET,

		/**
		 * Error on independent targets.
		 */
		ERROR_INDEPENDENT_TARGET
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
	IndependentTarget GetIndependentTarget() const
	{
		return fIndependentTarget;
	}
	IndependentTargetStrictness GetIndependentTargetStrictness() const
	{
		return fIndependentTargetStrictness;
	}

  private:
	EchoTrailingSpace fEchoTrailingSpace;
	PathRootReplacerSlash fPathRootReplacerSlash;
	BrokenSubscriptJoin fBrokenSubscriptJoin;
	JoinCaseOperator fJoinCaseOperator;
	IndependentTarget fIndependentTarget;
	IndependentTargetStrictness fIndependentTargetStrictness;
};

} // namespace ham::behavior

#endif // HAM_BEHAVIOR_BEHAVIOR_HPP
