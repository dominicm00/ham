/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TESTS_STRING_LIST_TEST_HPP
#define HAM_TESTS_STRING_LIST_TEST_HPP

#include "test/TestFixture.hpp"

namespace ham::tests
{

class StringListTest : public test::TestFixture
{
  public:
	void Constructor();
	void ElementAccess();
	void IsTrue();
	void SubList();
	void Contains();
	void Constants();
	void Comparison();
	void Assignment();
	void Concatenation();
	void Clear();
	void Join();
	void JoinWithSeparator();
	void Multiply();
	void Iteration();

	// declare tests
	HAM_ADD_TEST_CASES(
		StringListTest,
		14,
		Constructor,
		ElementAccess,
		IsTrue,
		SubList,
		Contains,
		Constants,
		Comparison,
		Assignment,
		Concatenation,
		Clear,
		Join,
		JoinWithSeparator,
		Multiply,
		Iteration
	)
};

} // namespace ham::tests

#endif // HAM_TESTS_STRING_LIST_TEST_HPP
