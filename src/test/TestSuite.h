/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TEST_TEST_SUITE_H
#define HAM_TEST_TEST_SUITE_H


#include <vector>

#include "test/Test.h"
#include "test/RunnableTest.h"


namespace test {


typedef std::vector<Test*> TestList;


class TestSuite : public Test {
public:
								TestSuite(const std::string& name = "");
	virtual						~TestSuite();

			int					CountTests() const
									{ return fTests.size(); }
			Test*				TestAt(int index) const;
			Test*				GetTest(const std::string& name) const;

			void				AddTest(Test* test);

private:
			TestList			fTests;
};


struct TestSuiteBuilder {
	TestSuiteBuilder(TestSuite& testSuite,
		TestSuiteBuilder* parentBuilder = NULL)
		:
		fTestSuite(testSuite),
		fParentBuilder(parentBuilder)
	{
	}

	template<typename TestFixture>
	TestSuiteBuilder& Add()
	{
		fTestSuite.AddTest(new GenericRunnableTest<TestFixture>());
		return *this;
	}

	TestSuiteBuilder AddSuite(const std::string& name)
	{
		TestSuite* testSuite = new TestSuite(name);
		fTestSuite.AddTest(testSuite);
		return TestSuiteBuilder(*testSuite, this);
	}

	TestSuiteBuilder& End()
	{
		return *fParentBuilder;
	}

private:
	TestSuite&			fTestSuite;
	TestSuiteBuilder*	fParentBuilder;
};


} // namespace test


#endif // HAM_TEST_TEST_SUITE_H
