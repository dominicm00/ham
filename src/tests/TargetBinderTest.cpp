/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "tests/TargetBinderTest.h"

#include "code/EvaluationContext.h"
#include "code/TargetBinder.h"
#include "data/MakeTarget.h"
#include "data/TargetPool.h"
#include "data/VariableDomain.h"


namespace ham {
namespace tests {


using code::TargetBinder;
using data::String;
using data::StringList;
using data::StringPart;


void
TargetBinderTest::Bind()
{
	// create a temporary directory where we can play
	TestFixture::TemporaryDirectoryCreator temporaryDirectoryCreator;
	std::string baseDirectory = temporaryDirectoryCreator.Create(true);

	// create a few directories and files
	CreateFile((baseDirectory + "/foo").c_str(), "");
	CreateFile((baseDirectory + "/subdir1/bar").c_str(), "");
	CreateFile((baseDirectory + "/subdir2/subdir3/foo").c_str(), "");

	struct TestData {
		const char*	target;
		StringList	targetLocate;
		StringList	targetSearch;
		StringList	globalLocate;
		StringList	globalSearch;
		std::string	boundPath;
	};

	const TestData testData[] = {
		{
			"foo",
			StringList(),
			StringList(),
			StringList(),
			StringList(),
			"foo"
		},
		{
			"foo",
			MakeStringList("."),
			StringList(),
			StringList(),
			StringList(),
			"./foo"
		},
		{
			"foo",
			MakeStringList(".."),
			StringList(),
			StringList(),
			StringList(),
			"../foo"
		},
		{
			"foo",
			MakeStringList((baseDirectory + "/subdir1").c_str()),
			StringList(),
			StringList(),
			StringList(),
			baseDirectory + "/subdir1/foo"
		},
		{
			"foo",
			MakeStringList((baseDirectory + "/subdir2/subdir3").c_str()),
			StringList(),
			StringList(),
			StringList(),
			baseDirectory + "/subdir2/subdir3/foo"
		},
		{
			"foo",
			MakeStringList((baseDirectory + "/subdir1").c_str(),
				(baseDirectory + "/subdir2/subdir3").c_str()),
			StringList(),
			StringList(),
			StringList(),
			baseDirectory + "/subdir1/foo"
		},
		{
			"foo",
			StringList(),
			MakeStringList((baseDirectory + "/subdir1").c_str()),
			StringList(),
			StringList(),
			"foo"
		},
		{
			"foo",
			StringList(),
			MakeStringList((baseDirectory + "/subdir1").c_str(),
				(baseDirectory + "/subdir2/subdir3").c_str()),
			StringList(),
			StringList(),
			baseDirectory + "/subdir2/subdir3/foo"
		},
		{
			"foo",
			StringList(),
			StringList(),
			MakeStringList("."),
			StringList(),
			"./foo"
		},
		{
			"foo",
			StringList(),
			StringList(),
			MakeStringList(".."),
			StringList(),
			"../foo"
		},
		{
			"foo",
			StringList(),
			StringList(),
			MakeStringList((baseDirectory + "/subdir1").c_str()),
			StringList(),
			baseDirectory + "/subdir1/foo"
		},
		{
			"foo",
			StringList(),
			StringList(),
			MakeStringList((baseDirectory + "/subdir2/subdir3").c_str()),
			StringList(),
			baseDirectory + "/subdir2/subdir3/foo"
		},
		{
			"foo",
			StringList(),
			StringList(),
			MakeStringList((baseDirectory + "/subdir1").c_str(),
				(baseDirectory + "/subdir2/subdir3").c_str()),
			StringList(),
			baseDirectory + "/subdir1/foo"
		},
		{
			"foo",
			StringList(),
			StringList(),
			StringList(),
			MakeStringList((baseDirectory + "/subdir1").c_str()),
			"foo"
		},
		{
			"foo",
			StringList(),
			StringList(),
			StringList(),
			MakeStringList((baseDirectory + "/subdir1").c_str(),
				(baseDirectory + "/subdir2/subdir3").c_str()),
			baseDirectory + "/subdir2/subdir3/foo"
		},
		{
			"foo",
			MakeStringList("somewhere"),
			MakeStringList("."),
			MakeStringList("elsewhere"),
			MakeStringList("subdir2/subdir3"),
			"somewhere/foo"
		},
		{
			"foo",
			StringList(),
			MakeStringList("."),
			MakeStringList("elsewhere"),
			MakeStringList("subdir2/subdir3"),
			"elsewhere/foo"
		},
		{
			"foo",
			StringList(),
			MakeStringList("."),
			StringList(),
			MakeStringList("subdir2/subdir3"),
			"./foo"
		},
		{
			"foo",
			StringList(),
			MakeStringList("subdir1"),
			StringList(),
			MakeStringList("subdir2/subdir3"),
			"foo"
		},
		{
			"foo",
			StringList(),
			StringList(),
			StringList(),
			MakeStringList("subdir2/subdir3"),
			"subdir2/subdir3/foo"
		},
		{
			"subdir3/foo",
			StringList(),
			StringList(),
			StringList(),
			MakeStringList("subdir2"),
			"subdir2/subdir3/foo"
		},
		{
			"/absolute/path/foo",
			MakeStringList("somewhere"),
			MakeStringList("."),
			MakeStringList("elsewhere"),
			MakeStringList("subdir2/subdir3"),
			"/absolute/path/foo"
		},
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		std::string targetName = testData[i].target;
		for (int k = 0; k < 2; k++) {
			if (k == 1)
				targetName = "<grist>" + targetName;

			data::VariableDomain globalVariables;
			data::TargetPool targets;
			data::Target* target = targets.LookupOrCreate(targetName.c_str());
			data::VariableDomain* targetVariables = target->Variables(true);

			if (!testData[i].targetLocate.IsEmpty())
				targetVariables->Set("LOCATE", testData[i].targetLocate);
			if (!testData[i].targetSearch.IsEmpty())
				targetVariables->Set("SEARCH", testData[i].targetSearch);
			if (!testData[i].globalLocate.IsEmpty())
				globalVariables.Set("LOCATE", testData[i].globalLocate);
			if (!testData[i].globalSearch.IsEmpty())
				globalVariables.Set("SEARCH", testData[i].globalSearch);

			code::EvaluationContext evalutationContext(globalVariables,
				targets);

			String boundPath;
			TargetBinder::Bind(evalutationContext, target, boundPath);

			HAM_TEST_ADD_INFO(
				HAM_TEST_EQUAL(boundPath, testData[i].boundPath.c_str()),
				"target: \"%s\", target locate: %s, target search: %s, "
				"global locate: %s, global search: %s", targetName.c_str(),
				ValueToString(testData[i].targetLocate).c_str(),
				ValueToString(testData[i].targetSearch).c_str(),
				ValueToString(testData[i].globalLocate).c_str(),
				ValueToString(testData[i].globalSearch).c_str())

			data::MakeTarget makeTarget(target);
			TargetBinder::Bind(evalutationContext, &makeTarget);

			HAM_TEST_ADD_INFO(
				HAM_TEST_VERIFY(makeTarget.IsBound())
				HAM_TEST_EQUAL(makeTarget.BoundPath(),
					testData[i].boundPath.c_str()),
				"target: \"%s\", target locate: %s, target search: %s, "
				"global locate: %s, global search: %s", targetName.c_str(),
				ValueToString(testData[i].targetLocate).c_str(),
				ValueToString(testData[i].targetSearch).c_str(),
				ValueToString(testData[i].globalLocate).c_str(),
				ValueToString(testData[i].globalSearch).c_str())
		}
	}
}


} // namespace tests
} // namespace ham
