/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "test/TestSuite.hpp"

namespace ham::test
{

TestSuite::TestSuite(const std::string& name)
	: Test(name)
{
}

TestSuite::~TestSuite()
{
	for (TestList::iterator it = fTests.begin(); it != fTests.end(); ++it)
		delete *it;
}

test::Test*
TestSuite::TestAt(int index) const
{
	return index >= 0 && (size_t)index < fTests.size() ? fTests[index] : nullptr;
}

test::Test*
TestSuite::GetTest(const std::string& name) const
{
	for (TestList::const_iterator it = fTests.begin(); it != fTests.end();
		 ++it) {
		Test* test = *it;
		if (test->Name() == name)
			return test;
	}

	return nullptr;
}

void
TestSuite::AddTest(Test* test)
{
	fTests.push_back(test);
	test->SetTestSuite(this);
}

} // namespace ham::test
