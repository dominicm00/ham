/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TEST_TEST_ENVIRONMENT_H
#define HAM_TEST_TEST_ENVIRONMENT_H


#include <string>

#include "behavior/Compatibility.h"


namespace ham {
namespace test {


class TestEnvironment {
public:
	TestEnvironment()
		:
		fCompatibility(behavior::COMPATIBILITY_HAM),
		fJamExecutable()
	{
	}

	behavior::Compatibility GetCompatibility() const
	{
		return fCompatibility;
	}

	void SetCompatibility(behavior::Compatibility compatibility)
	{
		fCompatibility = compatibility;
	}

	const std::string& JamExecutable() const
	{
		return fJamExecutable;
	}

	void SetJamExecutable(const std::string& executable)
	{
		fJamExecutable = executable;
	}

protected:
	behavior::Compatibility	fCompatibility;
	std::string				fJamExecutable;
};


} // namespace test
} // namespace ham


#endif // HAM_TEST_TEST_ENVIRONMENT_H
