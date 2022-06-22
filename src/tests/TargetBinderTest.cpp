/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "tests/TargetBinderTest.hpp"

#include "data/FileStatus.hpp"
#include "data/TargetBinder.hpp"
#include "data/TargetPool.hpp"
#include "data/VariableDomain.hpp"

namespace ham::tests
{

using data::FileStatus;
using data::String;
using data::StringList;
using data::StringPart;
using data::TargetBinder;
using data::Time;

void
TargetBinderTest::Bind()
{
	Time startTime = Time::Now();

	// create a temporary directory where we can play
	TestFixture::TemporaryDirectoryCreator temporaryDirectoryCreator;
	std::string baseDirectory = temporaryDirectoryCreator.Create(true);

	// create a few directories and files
	CreateFile((baseDirectory + "/foo").c_str(), "");
	CreateFile((baseDirectory + "/subdir1/bar").c_str(), "");
	CreateFile((baseDirectory + "/subdir2/subdir3/foo").c_str(), "");

	Time endTime = Time::Now();

	struct TestData {
		const char* target;
		StringList targetLocate;
		StringList targetSearch;
		StringList globalLocate;
		StringList globalSearch;
		std::string boundPath;
		FileStatus::Type type;
	};

	const TestData testData[] = {
		{"foo",
		 StringList(),
		 StringList(),
		 StringList(),
		 StringList(),
		 "foo",
		 FileStatus::FILE},
		{"foo",
		 MakeStringList("."),
		 StringList(),
		 StringList(),
		 StringList(),
		 "./foo",
		 FileStatus::FILE},
		{"foo",
		 MakeStringList(".."),
		 StringList(),
		 StringList(),
		 StringList(),
		 "../foo",
		 FileStatus::NONE},
		{"foo",
		 MakeStringList((baseDirectory + "/subdir1").c_str()),
		 StringList(),
		 StringList(),
		 StringList(),
		 baseDirectory + "/subdir1/foo",
		 FileStatus::NONE},
		{"foo",
		 MakeStringList((baseDirectory + "/subdir2/subdir3").c_str()),
		 StringList(),
		 StringList(),
		 StringList(),
		 baseDirectory + "/subdir2/subdir3/foo",
		 FileStatus::FILE},
		{"foo",
		 MakeStringList((baseDirectory + "/subdir1").c_str(),
						(baseDirectory + "/subdir2/subdir3").c_str()),
		 StringList(),
		 StringList(),
		 StringList(),
		 baseDirectory + "/subdir1/foo",
		 FileStatus::NONE},
		{"foo",
		 StringList(),
		 MakeStringList((baseDirectory + "/subdir1").c_str()),
		 StringList(),
		 StringList(),
		 "foo",
		 FileStatus::FILE},
		{"foo",
		 StringList(),
		 MakeStringList((baseDirectory + "/subdir1").c_str(),
						(baseDirectory + "/subdir2/subdir3").c_str()),
		 StringList(),
		 StringList(),
		 baseDirectory + "/subdir2/subdir3/foo",
		 FileStatus::FILE},
		{"foo",
		 StringList(),
		 StringList(),
		 MakeStringList("."),
		 StringList(),
		 "./foo",
		 FileStatus::FILE},
		{"foo",
		 StringList(),
		 StringList(),
		 MakeStringList(".."),
		 StringList(),
		 "../foo",
		 FileStatus::NONE},
		{"foo",
		 StringList(),
		 StringList(),
		 MakeStringList((baseDirectory + "/subdir1").c_str()),
		 StringList(),
		 baseDirectory + "/subdir1/foo",
		 FileStatus::NONE},
		{"foo",
		 StringList(),
		 StringList(),
		 MakeStringList((baseDirectory + "/subdir2/subdir3").c_str()),
		 StringList(),
		 baseDirectory + "/subdir2/subdir3/foo",
		 FileStatus::FILE},
		{"foo",
		 StringList(),
		 StringList(),
		 MakeStringList((baseDirectory + "/subdir1").c_str(),
						(baseDirectory + "/subdir2/subdir3").c_str()),
		 StringList(),
		 baseDirectory + "/subdir1/foo",
		 FileStatus::NONE},
		{"foo",
		 StringList(),
		 StringList(),
		 StringList(),
		 MakeStringList((baseDirectory + "/subdir1").c_str()),
		 "foo",
		 FileStatus::FILE},
		{"foo",
		 StringList(),
		 StringList(),
		 StringList(),
		 MakeStringList((baseDirectory + "/subdir1").c_str(),
						(baseDirectory + "/subdir2/subdir3").c_str()),
		 baseDirectory + "/subdir2/subdir3/foo",
		 FileStatus::FILE},
		{"foo",
		 MakeStringList("somewhere"),
		 MakeStringList("."),
		 MakeStringList("elsewhere"),
		 MakeStringList("subdir2/subdir3"),
		 "somewhere/foo",
		 FileStatus::NONE},
		{"foo",
		 StringList(),
		 MakeStringList("."),
		 MakeStringList("elsewhere"),
		 MakeStringList("subdir2/subdir3"),
		 "elsewhere/foo",
		 FileStatus::NONE},
		{"foo",
		 StringList(),
		 MakeStringList("."),
		 StringList(),
		 MakeStringList("subdir2/subdir3"),
		 "./foo",
		 FileStatus::FILE},
		{"foo",
		 StringList(),
		 MakeStringList("subdir1"),
		 StringList(),
		 MakeStringList("subdir2/subdir3"),
		 "foo",
		 FileStatus::FILE},
		{"foo",
		 StringList(),
		 StringList(),
		 StringList(),
		 MakeStringList("subdir2/subdir3"),
		 "subdir2/subdir3/foo",
		 FileStatus::FILE},
		{"subdir3/foo",
		 StringList(),
		 StringList(),
		 StringList(),
		 MakeStringList("subdir2"),
		 "subdir2/subdir3/foo",
		 FileStatus::FILE},
		{"/absolute/path/foo",
		 MakeStringList("somewhere"),
		 MakeStringList("."),
		 MakeStringList("elsewhere"),
		 MakeStringList("subdir2/subdir3"),
		 "/absolute/path/foo",
		 FileStatus::NONE},
		{"subdir3",
		 StringList(),
		 StringList(),
		 StringList(),
		 MakeStringList("subdir2"),
		 "subdir2/subdir3",
		 FileStatus::DIRECTORY},
		{"subdir3",
		 StringList(),
		 StringList(),
		 StringList(),
		 MakeStringList((baseDirectory + "/subdir2").c_str()),
		 baseDirectory + "/subdir2/subdir3",
		 FileStatus::DIRECTORY},
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

			String boundPath;
			FileStatus fileStatus;
			TargetBinder::Bind(globalVariables, target, boundPath, fileStatus);

			HAM_TEST_ADD_INFO(
				HAM_TEST_EQUAL(boundPath, testData[i].boundPath.c_str())
					HAM_TEST_EQUAL(fileStatus.GetType(), testData[i].type),
				"target: \"%s\", target locate: %s, target search: %s, "
				"global locate: %s, global search: %s",
				targetName.c_str(),
				ValueToString(testData[i].targetLocate).c_str(),
				ValueToString(testData[i].targetSearch).c_str(),
				ValueToString(testData[i].globalLocate).c_str(),
				ValueToString(testData[i].globalSearch).c_str())

			if (testData[i].type != FileStatus::NONE) {
				HAM_TEST_ADD_INFO(
					// The FS might not store the nanoseconds, so we only
					// compare the seconds.
					//				HAM_TEST_VERIFY(fileStatus.LastModifiedTime() >=
					//startTime) 				HAM_TEST_VERIFY(fileStatus.LastModifiedTime()
					//<= endTime),
					HAM_TEST_VERIFY(fileStatus.LastModifiedTime().Seconds()
									>= startTime.Seconds())
						HAM_TEST_VERIFY(fileStatus.LastModifiedTime().Seconds()
										<= endTime.Seconds()),
					"path: \"%s\", time: (%u, %u), start time: (%u, %u), "
					"end time: (%u, %u)",
					boundPath.ToCString(),
					(unsigned)fileStatus.LastModifiedTime().Seconds(),
					(unsigned)fileStatus.LastModifiedTime().NanoSeconds(),
					(unsigned)startTime.Seconds(),
					(unsigned)startTime.NanoSeconds(),
					(unsigned)endTime.Seconds(),
					(unsigned)endTime.NanoSeconds())
			}
		}
	}
}

} // namespace ham::tests
