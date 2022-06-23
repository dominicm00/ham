/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TEST_TEST_ENVIRONMENT_HPP
#define HAM_TEST_TEST_ENVIRONMENT_HPP

#include "behavior/Compatibility.hpp"

#include <string>

namespace ham::test
{

class TestEnvironment
{
  public:
	TestEnvironment()
		: fCompatibility(behavior::COMPATIBILITY_HAM),
		  fJamExecutable()
	{
	}

	behavior::Compatibility GetCompatibility() const { return fCompatibility; }

	void SetCompatibility(behavior::Compatibility compatibility)
	{
		fCompatibility = compatibility;
	}

	const std::string& JamExecutable() const { return fJamExecutable; }

	void SetJamExecutable(const std::string& executable)
	{
		fJamExecutable = executable;
	}

  protected:
	behavior::Compatibility fCompatibility;
	std::string fJamExecutable;
};

} // namespace ham::test

#endif // HAM_TEST_TEST_ENVIRONMENT_HPP
