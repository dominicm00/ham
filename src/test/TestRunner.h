/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TEST_TEST_RUNNER_H
#define HAM_TEST_TEST_RUNNER_H


#include <list>
#include <string>


namespace ham {
namespace test {


class RunnableTest;
class Test;
class TestEnvironment;
class TestSuite;


class TestRunner {
public:
								TestRunner();

			bool				AddTest(Test* test,
									const std::string& testCasePath
										= std::string());

			void				Run(TestEnvironment* environment);

private:
			struct TestIdentifier {
				TestIdentifier(Test* test = NULL, int testCaseIndex = -1)
					:
					fTest(test),
					fTestCaseIndex(testCaseIndex)
				{
				}

				bool IsValid() const
				{
					return fTest != NULL;
				}

				Test* GetTest() const
				{
					return fTest;
				}

				int TestCaseIndex() const
				{
					return fTestCaseIndex;
				}

			private:
				Test*	fTest;
				int		fTestCaseIndex;
			};

			typedef std::list<TestIdentifier> TestIdentifierList;

private:
			void				_RunTest(Test* test, int testCase);
			void				_RunTestCase(RunnableTest* test, int testCase);
			void				_InitFixture(RunnableTest* test);
			void				_CleanupFixture();

private:
			TestIdentifierList	fTestsToRun;
			TestEnvironment*	fEnvironment;
			RunnableTest*		fCurrentTest;
			void*				fCurrentTestFixture;
			size_t				fPassedTests;
			size_t				fFailedTests;
};


} // namespace test
} // namespace ham


#endif // HAM_TEST_TEST_RUNNER_H
