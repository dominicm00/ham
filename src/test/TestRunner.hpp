/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TEST_TEST_RUNNER_HPP
#define HAM_TEST_TEST_RUNNER_HPP

#include <list>
#include <string>

namespace ham
{
namespace test
{

class RunnableTest;
class Test;
class TestEnvironment;
class TestSuite;

class TestRunner
{
  public:
	TestRunner();

	bool AddTest(Test* test, const std::string& testCasePath = std::string());

	void Run(TestEnvironment* environment);

  private:
	struct TestIdentifier {
		TestIdentifier(Test* test = nullptr, int testCaseIndex = -1)
			: fTest(test),
			  fTestCaseIndex(testCaseIndex)
		{
		}

		bool IsValid() const { return fTest != nullptr; }

		Test* GetTest() const { return fTest; }

		int TestCaseIndex() const { return fTestCaseIndex; }

	  private:
		Test* fTest;
		int fTestCaseIndex;
	};

	typedef std::list<TestIdentifier> TestIdentifierList;

  private:
	void _RunTest(Test* test, int testCase);
	void _RunTestCase(RunnableTest* test, int testCase);
	void _InitFixture(RunnableTest* test);
	void _CleanupFixture();

  private:
	TestIdentifierList fTestsToRun;
	TestEnvironment* fEnvironment;
	RunnableTest* fCurrentTest;
	void* fCurrentTestFixture;
	size_t fPassedTests;
	TestIdentifierList fFailedTests;
	size_t fSkippedTests;
	size_t fExpectedlyFailedTests;
	TestIdentifierList fUnexpectedlyPassedTests;
};

} // namespace test
} // namespace ham

#endif // HAM_TEST_TEST_RUNNER_HPP
