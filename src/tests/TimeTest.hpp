/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TESTS_TIME_TEST_HPP
#define HAM_TESTS_TIME_TEST_HPP

#include "test/TestFixture.hpp"

namespace ham::tests
{

class TimeTest : public test::TestFixture
{
  public:
	void Constructor();
	void Comparison();
	void Now();

	// declare tests
	HAM_ADD_TEST_CASES(TimeTest, 3, Constructor, Comparison, Now)
};

} // namespace ham::tests

#endif // HAM_TESTS_TIME_TEST_HPP
