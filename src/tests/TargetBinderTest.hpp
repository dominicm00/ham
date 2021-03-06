/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TESTS_TARGET_BINDER_TEST_HPP
#define HAM_TESTS_TARGET_BINDER_TEST_HPP

#include "test/TestFixture.hpp"

namespace ham::tests
{

class TargetBinderTest : public test::TestFixture
{
  public:
	void Bind();

	// declare tests
	HAM_ADD_TEST_CASES(TargetBinderTest, 1, Bind)
};

} // namespace ham::tests

#endif // HAM_TESTS_TARGET_BINDER_TEST_HPP
