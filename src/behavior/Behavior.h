/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_BEHAVIOR_BEHAVIOR_H
#define HAM_BEHAVIOR_BEHAVIOR_H


#include "behavior/Compatibility.h"


namespace ham {
namespace behavior {


class Behavior {
public:
			enum EchoTrailingSpace {
				ECHO_TRAILING_SPACE,
					// Always append a slash at the end of an Echo line.
				ECHO_NO_TRAILING_SPACE
					// Don't append a slash at the end of an Echo line.
			};

			enum PathRootReplacerSlash {
				PATH_ROOT_REPLACER_SLASH_ALWAYS,
					// If the parameter is not empty, the root replacer always
					// appends '/' after the root component. Leads to duplicate
					// '/', when the parameter ends with a slash.
				PATH_ROOT_REPLACER_SLASH_AVOID_DUPLICATE
					// If the parameter is not empty and doesn't end with a '/',
					// the root replacer appends a '/' after the root component.
			};

			enum BrokenSubscriptJoin {
				BROKEN_SUBSCRIPT_JOIN,
					// A join operation with an end subscript less than the
					// number of list elements result in an empty list.
				NO_BROKEN_SUBSCRIPT_JOIN
					// Joins with subscripts work as expected.
			};

			enum JoinCaseOperator {
				JOIN_BEFORE_CASE_OPERATOR,
					// The join operator is executed before executing the
					// to-lower or to-upper operator, i.e. the join parameter
					// will be affected by the case operator as well.
				JOIN_AFTER_CASE_OPERATOR
					// The join operator is executed after executing the
					// to-lower or to-upper operator, i.e. the join parameter
					// won't be affected by the case operator.
			};

public:
								Behavior(Compatibility compatibility);

			EchoTrailingSpace	GetEchoTrailingSpace() const
									{ return fEchoTrailingSpace; }
			PathRootReplacerSlash GetPathRootReplacerSlash() const
									{ return fPathRootReplacerSlash; }
			BrokenSubscriptJoin	GetBrokenSubscriptJoin() const
									{ return fBrokenSubscriptJoin; }
			JoinCaseOperator	GetJoinCaseOperator() const
									{ return fJoinCaseOperator; }

private:
			EchoTrailingSpace	fEchoTrailingSpace;
			PathRootReplacerSlash fPathRootReplacerSlash;
			BrokenSubscriptJoin	fBrokenSubscriptJoin;
			JoinCaseOperator	fJoinCaseOperator;
};


}	// namespace behavior
}	// namespace ham


#endif	// HAM_BEHAVIOR_BEHAVIOR_H
