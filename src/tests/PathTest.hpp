/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TESTS_PATH_TEST_HPP
#define HAM_TESTS_PATH_TEST_HPP

#include "test/TestFixture.hpp"

namespace ham
{
namespace tests
{

class PathTest : public test::TestFixture
{
  public:
	void IsAbsolute();
	void RemoveGrist();
	void Make();
	void GetFileStatus();
	void PartsConstructor();
	void PartsSetters();
	void PartsPath();
	void PartsIsAbsolute();

	// declare tests
	HAM_ADD_TEST_CASES(PathTest,
					   8,
					   IsAbsolute,
					   RemoveGrist,
					   Make,
					   GetFileStatus,
					   PartsConstructor,
					   PartsSetters,
					   PartsPath,
					   PartsIsAbsolute)
};

} // namespace tests
} // namespace ham

#endif // HAM_TESTS_PATH_TEST_HPP
