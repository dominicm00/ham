/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "test/RunnableTest.h"


using test::RunnableTest;


RunnableTest::RunnableTest(const std::string& name)
	:
	Test(name)
{
}


std::string
RunnableTest::TestCaseAt(int index, bool fullyQualified) const
{
	if (index < 0 || (size_t)index >= fTestCaseNames.size())
		return std::string();

	return fullyQualified
		? FullyQualifiedName() + "::" + fTestCaseNames[index]
		: fTestCaseNames[index];
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
