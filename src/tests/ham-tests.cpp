/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "parser/ParseException.hpp"
#include "test/DataBasedTestParser.hpp"
#include "test/RunnableTest.hpp"
#include "test/TestRunner.hpp"
#include "test/TestSuite.hpp"
#include "tests/PathTest.hpp"
#include "tests/RegExpTest.hpp"
#include "tests/RulesetTest.hpp"
#include "tests/StringListTest.hpp"
#include "tests/StringPartTest.hpp"
#include "tests/StringTest.hpp"
#include "tests/TargetBinderTest.hpp"
#include "tests/TimeTest.hpp"
#include "tests/VariableExpansionTest.hpp"

#include <dirent.h>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace ham;

static void
print_usage(const char* programName, bool error)
{
	FILE* out = error ? stderr : stdout;
	fprintf(out, "Usage: %s [ <options> ] [ <tests> ]\n", programName);
	fprintf(out, "Options:\n");
	fprintf(out, "  -c <version>, --compatibility <version>\n");
	fprintf(
		out,
		"      Behave compatible to <version>, which is either of "
		"\"jam\" (plain jam\n"
	);
	fprintf(
		out,
		"      2.5), \"boost\" (Boost.Jam), or \"ham\" (Ham, the "
		"default).\n"
	);
	fprintf(out, "  -d <directory>, --data <directory>\n");
	fprintf(
		out,
		"      Use test data directory <directory>. By default the "
		"program tries to\n"
	);
	fprintf(
		out,
		"      find the test data directory relative to the current "
		"working directory.\n"
	);
	fprintf(out, "  -h, --help\n");
	fprintf(out, "      Print this usage message.\n");
	fprintf(out, "  -j <executable>, --jam <executable>\n");
	fprintf(
		out,
		"      Use the jam executable <executable> to run the tests "
		"supporting it.\n"
	);
	fprintf(out, "  -l, --list\n");
	fprintf(out, "      List all available tests and exit.\n");
}

static void
print_usage_end_exit(const char* programName, bool error)
{
	print_usage(programName, error);
	exit(error ? 1 : 0);
}

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
	} else if (test::RunnableTest* runnableTest = dynamic_cast<test::RunnableTest*>(test)) {
		int count = runnableTest->CountTestCases();
		for (int i = 0; i < count; i++) {
			printf(
				"%*s%s\n",
				2 * (level + 1),
				"",
				runnableTest->TestCaseAt(i).c_str()
			);
		}
	}
}

static std::string
find_test_data_directory()
{
	// get the current working directory
	char buffer[10240];
	if (getcwd(buffer, sizeof(buffer)) == nullptr)
		return std::string();

	std::string directory = buffer;
	for (;;) {
		// test if the directory contains a subdirectory "testdata"
		std::string testDataDirectory = directory + "/testdata";
		struct stat st;
		if (stat(testDataDirectory.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
			return testDataDirectory;

		// go up one directory level
		size_t slashIndex = directory.rfind('/');
		if (slashIndex == std::string::npos || slashIndex == 0)
			return std::string();

		directory.resize(slashIndex);
	}
}

static void
add_data_based_tests_recursive(
	test::TestSuite& testSuite,
	const std::string& directory
)
{
	DIR* dir = opendir(directory.c_str());
	if (dir == nullptr)
		return;

	while (struct dirent* entry = readdir(dir)) {
		std::string name = entry->d_name;

		// ignore ., .., and hidden files
		if (name[0] == '.')
			continue;

		std::string path = directory + '/' + name;
		struct stat st;
		if (stat(path.c_str(), &st) != 0)
			continue;

		if (S_ISDIR(st.st_mode)) {
			std::unique_ptr<test::TestSuite> subTestSuite(
				new test::TestSuite(name)
			);
			add_data_based_tests_recursive(*subTestSuite, path);
			if (subTestSuite->CountTests() > 0) {
				testSuite.AddTest(subTestSuite.release());
			}
		} else if (S_ISREG(st.st_mode)) {
			try {
				testSuite.AddTest(test::DataBasedTestParser().Parse(path.c_str()
				));
			} catch (parser::ParseException& exception) {
				fprintf(
					stderr,
					"add_data_based_tests_recursive(): %s:%zu:%zu "
					"Parse exception: %s\n",
					path.c_str(),
					exception.Position().Line() + 1,
					exception.Position().Column() + 1,
					exception.Message()
				);
				exit(1);
			}
		}
	}

	closedir(dir);
}

static void
add_data_based_tests(test::TestSuite& testSuite, const std::string& directory)
{
	// get the simple tests
	std::unique_ptr<test::TestSuite> dataBasedTestSuite(
		new test::TestSuite("DataBased")
	);
	add_data_based_tests_recursive(*dataBasedTestSuite, directory);
	if (dataBasedTestSuite->CountTests() > 0) {
		testSuite.AddTest(dataBasedTestSuite.release());
	}
}

int
main(int argc, const char* const* argv)
{
	using namespace tests;

	test::TestSuite testSuite;
	test::TestSuiteBuilder(testSuite)
		.AddSuite("Data")
		.Add<PathTest>()
		.Add<RegExpTest>()
		.Add<RulesetTest>()
		.Add<StringListTest>()
		.Add<StringPartTest>()
		.Add<StringTest>()
		.Add<TargetBinderTest>()
		.Add<TimeTest>()
		.End()
		.AddSuite("Code")
		.Add<VariableExpansionTest>()
		.End();

	// parse arguments
	std::string testDataDirectory;
	std::string jamExecutable;
	bool listOnly = false;
	behavior::Compatibility compatibility = behavior::COMPATIBILITY_HAM;
	bool explicitCompatibility = false;

	int argi = 1;
	while (argi < argc) {
		const char* arg = argv[argi];
		if (arg[0] != '-')
			break;

		argi++;
		arg++;

		if (*arg == '-') {
			// long ("--") option -- map to short option
			if (strcmp(arg, "-compatibility") == 0)
				arg = "c";
			else if (strcmp(arg, "-data") == 0)
				arg = "d";
			else if (strcmp(arg, "-help") == 0)
				arg = "h";
			else if (strcmp(arg, "-jam") == 0)
				arg = "j";
			else if (strcmp(arg, "-list") == 0)
				arg = "l";
			else
				print_usage_end_exit(argv[0], true);
		}

		// short ("-") option(s)
		for (; *arg != '\0'; arg++) {
			switch (*arg) {
				case 'c': {
					if (argi == argc)
						print_usage_end_exit(argv[0], true);

					const char* compatibilityString = argv[argi++];
					if (strcmp(compatibilityString, "jam") == 0) {
						compatibility = behavior::COMPATIBILITY_JAM;
					} else if (strcmp(compatibilityString, "boost") == 0) {
						compatibility = behavior::COMPATIBILITY_BOOST_JAM;
					} else if (strcmp(compatibilityString, "ham") == 0) {
						compatibility = behavior::COMPATIBILITY_HAM;
					} else {
						fprintf(
							stderr,
							"Error: Invalid argument for "
							"compatibility option: \"%s\"\n",
							compatibilityString
						);
						exit(1);
					}
					explicitCompatibility = true;
					break;
				}
				case 'd':
					if (argi == argc)
						print_usage_end_exit(argv[0], true);
					testDataDirectory = argv[argi++];
					break;
				case 'h':
					print_usage_end_exit(argv[0], false);
					break;
				case 'j':
					if (argi == argc)
						print_usage_end_exit(argv[0], true);
					jamExecutable = argv[argi++];

					// If not explicitly given, guess the jam version.
					if (!explicitCompatibility) {
						const char* slash = strrchr(jamExecutable.c_str(), '/');
						const char* baseName = slash != nullptr
							? slash + 1
							: jamExecutable.c_str();
						if (strcmp(baseName, "jam") == 0)
							compatibility = behavior::COMPATIBILITY_JAM;
						else if (strcmp(baseName, "bjam") == 0)
							compatibility = behavior::COMPATIBILITY_BOOST_JAM;
					}
					break;
				case 'l':
					listOnly = true;
					break;
				default:
					print_usage_end_exit(argv[0], true);
			}
		}
	}

	// add/find test data directory
	if (testDataDirectory.empty())
		testDataDirectory = find_test_data_directory();
	if (!testDataDirectory.empty())
		add_data_based_tests(testSuite, testDataDirectory.c_str());

	if (listOnly) {
		list_tests(&testSuite);
		exit(0);
	}

	// The remaining arguments are test names. If none are given, run all tests.
	test::TestRunner testRunner;
	if (argi < argc) {
		while (argi < argc) {
			const char* testName = argv[argi++];
			if (!testRunner.AddTest(&testSuite, testName)) {
				fprintf(
					stderr,
					"Unknown test \"%s\". Use option \"--list\" to "
					"list available tests.\n",
					testName
				);
				exit(1);
			}
		}
	} else
		testRunner.AddTest(&testSuite);

	// run tests
	test::TestEnvironment environment;
	environment.SetCompatibility(compatibility);
	environment.SetJamExecutable(jamExecutable);
	testRunner.Run(&environment);

	return 0;
}
