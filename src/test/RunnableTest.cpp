/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "test/RunnableTest.h"

namespace ham
{
namespace test
{

RunnableTest::RunnableTest(const std::string& name, bool isJammable)
	: Test(name),
	  fIsJammable(isJammable)
{
}

std::string
RunnableTest::TestCaseAt(int index, bool fullyQualified) const
{
	if (index < 0 || (size_t)index >= fTestCaseNames.size())
		return std::string();

	const std::string& testCaseName = fTestCaseNames[index];
	if (!fullyQualified)
		return testCaseName;

	std::string testName = FullyQualifiedName();
	return testCaseName.empty() ? testName : testName + "::" + testCaseName;
}

int
RunnableTest::IndexOfTestCase(const std::string& testCase) const
{
	int count = CountTestCases();
	for (int i = 0; i < count; ++i) {
		if (testCase == fTestCaseNames[i])
			return i;
	}

	return -1;
}

uint32_t
RunnableTest::TestCaseCompatibility(int index,
									bool& _supportedByHam,
									uint32_t& _skipMask)
{
	_supportedByHam = true;
	_skipMask = 0;

	return behavior::COMPATIBILITY_MASK_ALL;
}

} // namespace test
} // namespace ham
