/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include <stdio.h>

#include "test/RunnableTest.h"
#include "test/TestRunner.h"
#include "test/TestSuite.h"
#include "tests/StringListTest.h"
#include "tests/StringTest.h"
#include "tests/VariableExpansionTest.h"


using namespace ham;


#if 0
static void
list_tests(test::Test* test, int level = 0)
{
	if (level == 0)
		printf("Tests:\n");
	else
		printf("%*s%s\n", 2 * level, "", test->Name().c_str());

	if (test::TestSuite* testSuite = dynamic_cast<test::TestSuite*>(test)) {
		int count = testSuite->CountTests();
		for (int i = 0; i < count; i++)
			list_tests(testSuite->TestAt(i), level + 1);
	} else if (test::RunnableTest* runnableTest
			= dynamic_cast<test::RunnableTest*>(test)) {
		int count = runnableTest->CountTestCases();
		for (int i = 0; i < count; i++) {
			printf("%*s%s\n", 2 * (level + 1), "",
				runnableTest->TestCaseAt(i).c_str());
		}
	}
}
#endif


int
main(int argc, const char* const* argv)
{
	using namespace tests;

	test::TestSuite testSuite;
	test::TestSuiteBuilder(testSuite)
		.AddSuite("Data")
			.Add<StringTest>()
			.Add<StringListTest>()
		.End()
		.AddSuite("Code")
			.Add<VariableExpansionTest>()
		.End();

//	list_tests(&testSuite);

	test::TestRunner testRunner;
	testRunner.AddTest(&testSuite);

	test::TestEnvironment environment;
	testRunner.Run(&environment);

	return 0;
}
