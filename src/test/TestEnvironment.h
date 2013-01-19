/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TEST_TEST_ENVIRONMENT_H
#define HAM_TEST_TEST_ENVIRONMENT_H


#include <string>


namespace ham {
namespace test {


class TestEnvironment {
public:
	TestEnvironment()
		:
		fJamExecutable()
	{
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
	std::string fJamExecutable;
};


} // namespace test
} // namespace ham


#endif // HAM_TEST_TEST_ENVIRONMENT_H
