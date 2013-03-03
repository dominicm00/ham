/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "tests/PathTest.h"

#include "data/FileStatus.h"
#include "data/Path.h"
#include "data/String.h"


namespace ham {
namespace tests {


using data::Path;
using data::String;
using data::StringPart;


void
PathTest::IsAbsolute()
{
	struct TestData {
		const char*	string;
		size_t		offset;
		size_t		length;
		bool		isAbsolute;
	};

	const TestData testData[] = {
		{ "",						0,	0,	false },
		{ "/",						0,	0,	false },
		{ "/",						0,	1,	true },
		{ "foo",					0,	3,	false },
		{ "foobar",					2,	3,	false },
		{ "some/path/foo",			0,	13,	false },
		{ "some/path/foo",			4,	9,	true },
		{ "/some/path/foo",			0,	14,	true },
		{ "/some/path/foo",			1,	13,	false },
		{ "/some/path/foo",			5,	5,	true },
		{ "/some/path/foo",			1,	5,	false },
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		HAM_TEST_VERIFY(strlen(testData[i].string)
			>= testData[i].offset + testData[i].length)

		StringPart path(testData[i].string + testData[i].offset,
			testData[i].length);
		HAM_TEST_ADD_INFO(
			HAM_TEST_EQUAL(Path::IsAbsolute(path), testData[i].isAbsolute),
			"string: \"%s\", offset: %zu, length: %zu", testData[i].string,
			testData[i].offset, testData[i].length)
	}
}


void
PathTest::RemoveGrist()
{
	struct TestData {
		const char*	string;
		size_t		offset;
		size_t		length;
		const char*	result;
	};

	const TestData testData[] = {
		{ "",						0,	0,	"" },
		{ "/",						0,	0,	"" },
		{ "/",						0,	1,	"/" },
		{ "foo",					0,	3,	"foo" },
		{ "foobar",					2,	3,	"oba" },
		{ "some/path/foo",			0,	13,	"some/path/foo" },
		{ "some/path/foo",			4,	9,	"/path/foo" },
		{ "/some/path/foo",			0,	14,	"/some/path/foo" },
		{ "/some/path/foo",			1,	13,	"some/path/foo" },
		{ "/some/path/foo",			5,	5,	"/path" },
		{ "/some/path/foo",			1,	5,	"some/" },
		{ "<grist>",				0,	7,	"" },
		{ "<grist>/",				0,	7,	"" },
		{ "<grist>/",				0,	8,	"/" },
		{ "<grist>foo",				0,	10,	"foo" },
		{ "<grist>foobar",			9,	3,	"oba" },
		{ "<grist>some/path/foo",	0,	20,	"some/path/foo" },
		{ "<grist>some/path/foo",	11,	9,	"/path/foo" },
		{ "<grist>/some/path/foo",	0,	21,	"/some/path/foo" },
		{ "<grist>/some/path/foo",	8,	13,	"some/path/foo" },
		{ "<grist>/some/path/foo",	12,	5,	"/path" },
		{ "<grist>/some/path/foo",	8,	5,	"some/" },
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		HAM_TEST_VERIFY(strlen(testData[i].string)
			>= testData[i].offset + testData[i].length)

		StringPart path(testData[i].string + testData[i].offset,
			testData[i].length);
		HAM_TEST_ADD_INFO(
			HAM_TEST_EQUAL(Path::RemoveGrist(path).ToStlString(),
				testData[i].result),
			"string: \"%s\", offset: %zu, length: %zu", testData[i].string,
			testData[i].offset, testData[i].length)
	}
}


void
PathTest::Make()
{
	struct TestData {
		const char*	head;
		const char*	tail;
		const char*	result;
	};

	const TestData testData[] = {
		{ "", "",							"" },
		{ "foo", "",						"foo" },
		{ "", "foo",						"foo" },
		{ "/", "",							"/" },
		{ "", "/",							"/" },
		{ "foo", "bar",						"foo/bar" },
		{ "foo/", "bar",					"foo/bar" },
		{ "foo/", "bar/",					"foo/bar/" },
		{ "foo", "/bar",					"/bar" },
		{ "/foo", "/bar",					"/bar" },
		{ "/foo", "bar/foobar",				"/foo/bar/foobar" },
	};

	const char* const testDataContext[] = {
		"",
		"xyz",
		"xyz/",
		"xyz/abc",
		"/xyz",
		"/xyz/abc",
		"/xyz/abc/"
	};
	const size_t testDataContextCount
		= sizeof(testDataContext) / sizeof(testDataContext[0]);

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		for (size_t headPrefix = 0; headPrefix < testDataContextCount;
			headPrefix++) {
			for (size_t headSuffix = 0; headSuffix < testDataContextCount;
				headSuffix++) {
				std::string headString = std::string()
					+ testDataContext[headPrefix] + testData[i].head
					+ testDataContext[headSuffix];
				StringPart head(
					headString.c_str() + strlen(testDataContext[headPrefix]),
					strlen(testData[i].head));
				for (size_t tailPrefix = 0; tailPrefix < testDataContextCount;
					tailPrefix++) {
					for (size_t tailSuffix = 0;
						tailSuffix < testDataContextCount; tailSuffix++) {
						std::string tailString = std::string()
							+ testDataContext[tailPrefix] + testData[i].tail
							+ testDataContext[tailSuffix];
						StringPart tail(
							tailString.c_str()
								+ strlen(testDataContext[tailPrefix]),
							strlen(testData[i].tail));
						HAM_TEST_ADD_INFO(
							HAM_TEST_EQUAL(Path::Make(head, tail).ToStlString(),
								testData[i].result),
							"head: \"%s\", tail: \"%s\"", testData[i].head,
							testData[i].tail)
					}
				}
			}
		}
	}
}


void
PathTest::GetFileStatus()
{
	using data::FileStatus;
	using data::Time;

	Time startTime = Time::Now();

	// create a temporary directory where we can play
	TestFixture::TemporaryDirectoryCreator temporaryDirectoryCreator;
	std::string baseDirectory = temporaryDirectoryCreator.Create(true);

	// create a few directories and files
	CreateFile((baseDirectory + "/foo").c_str(), "");
	CreateFile((baseDirectory + "/subdir1/bar").c_str(), "");
	CreateFile((baseDirectory + "/subdir2/subdir3/foobar").c_str(), "");

	Time endTime = Time::Now();

	struct TestData {
		std::string			path;
		FileStatus::Type	type;
		bool				checkTime;
	};

	const TestData testData[] = {
		{ "",											FileStatus::NONE,
			true },
		{ ".",											FileStatus::DIRECTORY,
			true },
		{ "..",											FileStatus::DIRECTORY,
			false },
		{ "foo",										FileStatus::FILE,
			true },
		{ "bar",										FileStatus::NONE,
			true },
		{ "subdir1/foo",								FileStatus::NONE,
			true },
		{ "subdir3/foo",								FileStatus::NONE,
			true },
		{ "subdir1/bar",								FileStatus::FILE,
			true },
		{ "subdir2/subdir3/foobar",						FileStatus::FILE,
			true },
		{ "subdir1/.",									FileStatus::DIRECTORY,
			true },
		{ "subdir1/..",									FileStatus::DIRECTORY,
			true },
		{ "subdir2/../subdir1/bar",						FileStatus::FILE,
			true },
		{ "subdir2/../subdir1/foo",						FileStatus::NONE,
			true },
		{ baseDirectory,								FileStatus::DIRECTORY,
			true },
		{ baseDirectory + "/.",							FileStatus::DIRECTORY,
			true },
		{ baseDirectory + "/..",						FileStatus::DIRECTORY,
			false },
		{ baseDirectory + "/foo",						FileStatus::FILE,
			true },
		{ baseDirectory + "/bar",						FileStatus::NONE,
			true },
		{ baseDirectory + "/subdir1/foo",				FileStatus::NONE,
			true },
		{ baseDirectory + "/subdir3/foo",				FileStatus::NONE,
			true },
		{ baseDirectory + "/subdir1/bar",				FileStatus::FILE,
			true },
		{ baseDirectory + "/subdir2/subdir3/foobar",	FileStatus::FILE,
			true },
		{ baseDirectory + "/subdir1/.",					FileStatus::DIRECTORY,
			true },
		{ baseDirectory + "/subdir1/..",				FileStatus::DIRECTORY,
			true },
		{ baseDirectory + "/subdir2/../subdir1/bar",	FileStatus::FILE,
			true },
		{ "baseDirectory + /subdir2/../subdir1/foo",	FileStatus::NONE,
			true },
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		const char* path = testData[i].path.c_str();

		FileStatus fileStatus;
		HAM_TEST_ADD_INFO(
			HAM_TEST_EQUAL(Path::Exists(path),
				testData[i].type != FileStatus::NONE)
			HAM_TEST_EQUAL(Path::GetFileStatus(path, fileStatus),
				testData[i].type != FileStatus::NONE)
			HAM_TEST_EQUAL(fileStatus.GetType(), testData[i].type),
			"path: \"%s\"", path)

		if (testData[i].type != FileStatus::NONE && testData[i].checkTime) {
			HAM_TEST_ADD_INFO(
// The FS might not store the nanoseconds, so we only compare the seconds.
//				HAM_TEST_VERIFY(fileStatus.LastModifiedTime() >= startTime)
//				HAM_TEST_VERIFY(fileStatus.LastModifiedTime() <= endTime),
				HAM_TEST_VERIFY(fileStatus.LastModifiedTime().Seconds()
					>= startTime.Seconds())
				HAM_TEST_VERIFY(fileStatus.LastModifiedTime().Seconds()
					<= endTime.Seconds()),
				"path: \"%s\", time: (%u, %u), start time: (%u, %u), "
				"end time: (%u, %u)", path,
				(unsigned)fileStatus.LastModifiedTime().Seconds(),
				(unsigned)fileStatus.LastModifiedTime().NanoSeconds(),
				(unsigned)startTime.Seconds(),
				(unsigned)startTime.NanoSeconds(), (unsigned)endTime.Seconds(),
				(unsigned)endTime.NanoSeconds())
		}
	}
}


void
PathTest::PartsConstructor()
{
	struct TestData {
		const char*	path;
		const char*	grist;
		const char*	root;
		const char*	directory;
		const char*	baseName;
		const char*	suffix;
		const char*	archiveMember;
	};

	const TestData testData[] = {
		{ "", "", "", "", "", "", "" },
		{ "foo", "", "", "", "foo", "", "" },
		{ "bar.a", "", "", "", "bar", ".a", "" },
		{ "foobar.bee", "", "", "", "foobar", ".bee", "" },
		{ "path/foo", "", "", "path", "foo", "", "" },
		{ "path/foo.a", "", "", "path", "foo", ".a", "" },
		{ "some/path/bar.bee", "", "", "some/path", "bar", ".bee", "" },
		{ "/an/absolute/path/foobar.so", "", "", "/an/absolute/path", "foobar",
			".so", "" },
		{ "/", "", "", "/", "", "", "" },
		{ "<grist>", "<grist>", "", "", "", "", "" },
		{ "<>foo", "<>", "", "", "foo", "", "" },
		{ "<g>bar.a", "<g>", "", "", "bar", ".a", "" },
		{ "<gr>foobar.bee", "<gr>", "", "", "foobar", ".bee", "" },
		{ "<gri>path/foo", "<gri>", "", "path", "foo", "", "" },
		{ "<gris>path/foo.a", "<gris>", "", "path", "foo", ".a", "" },
		{ "<grist>some/path/bar.bee", "<grist>", "", "some/path", "bar", ".bee",
			"" },
		{ "<gristt>/an/absolute/path/foobar.so", "<gristt>", "",
			"/an/absolute/path", "foobar", ".so", "" },
		{ "<g>/", "<g>", "", "/", "", "", "" },
		{ "(archive)", "", "", "", "", "", "archive" },
		{ "(member.o)", "", "", "", "", "", "member.o" },
		{ "foo(archive)", "", "", "", "foo", "", "archive" },
		{ "bar.a(archive)", "", "", "", "bar", ".a", "archive" },
		{ "foobar.bee(member.o)", "", "", "", "foobar", ".bee", "member.o" },
		{ "path/foo(archive)", "", "", "path", "foo", "", "archive" },
		{ "path/foo.a(archive)", "", "", "path", "foo", ".a", "archive" },
		{ "some/path/bar.bee(member.o)", "", "", "some/path", "bar", ".bee",
			"member.o" },
		{ "/an/absolute/path/foobar.so(member.o)", "", "", "/an/absolute/path",
			"foobar",".so", "member.o" },
		{ "foo(archive)x", "", "", "", "foo(archive)x", "", "" },
		{ "bar.a(x/archive)", "", "", "bar.a(x", "archive)", "", "" },
		{ "/(archive.o)", "", "", "/", "", "", "archive.o" },
		{ "<>(archive)", "<>", "", "", "", "", "archive" },
		{ "<g>(member.o)", "<g>", "", "", "", "", "member.o" },
		{ "<gr>foo(archive)", "<gr>", "", "", "foo", "", "archive" },
		{ "<gri>bar.a(archive)", "<gri>", "", "", "bar", ".a", "archive" },
		{ "<gris>foobar.bee(member.o)", "<gris>", "", "", "foobar", ".bee",
			"member.o" },
		{ "<grist>path/foo(archive)", "<grist>", "", "path", "foo", "",
			"archive" },
		{ "<>path/foo.a(archive)", "<>", "", "path", "foo", ".a", "archive" },
		{ "<g>some/path/bar.bee(member.o)", "<g>", "", "some/path", "bar",
			".bee", "member.o" },
		{ "<gr>/an/absolute/path/foobar.so(member.o)", "<gr>", "",
			"/an/absolute/path", "foobar",".so", "member.o" },
		{ "<gri>foo(archive)x", "<gri>", "", "", "foo(archive)x", "", "" },
		{ "<gris>bar.a(x/archive)", "<gris>", "", "bar.a(x", "archive)", "",
			"" },
		{ "<grist>/(archive.o)", "<grist>", "", "/", "", "", "archive.o" },
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		// constructor
		{
			Path::Parts parts(StringPart(testData[i].path));
			HAM_TEST_ADD_INFO(
				HAM_TEST_EQUAL(parts.Grist().ToStlString(), testData[i].grist)
				HAM_TEST_EQUAL(parts.Root().ToStlString(), testData[i].root)
				HAM_TEST_EQUAL(parts.Directory().ToStlString(),
					testData[i].directory)
				HAM_TEST_EQUAL(parts.BaseName().ToStlString(),
					testData[i].baseName)
				HAM_TEST_EQUAL(parts.Suffix().ToStlString(), testData[i].suffix)
				HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
					testData[i].archiveMember),
				"path: \"%s\"", testData[i].path);
		}

		// SetTo()
		{
			Path::Parts parts;
			parts.SetTo(StringPart(testData[i].path));
			HAM_TEST_ADD_INFO(
				HAM_TEST_EQUAL(parts.Grist().ToStlString(), testData[i].grist)
				HAM_TEST_EQUAL(parts.Root().ToStlString(), testData[i].root)
				HAM_TEST_EQUAL(parts.Directory().ToStlString(),
					testData[i].directory)
				HAM_TEST_EQUAL(parts.BaseName().ToStlString(),
					testData[i].baseName)
				HAM_TEST_EQUAL(parts.Suffix().ToStlString(), testData[i].suffix)
				HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
					testData[i].archiveMember),
				"path: \"%s\"", testData[i].path);
		}

		{
			Path::Parts parts(StringPart("<a>/b/c.d(e)"));
			parts.SetRoot(StringPart("f"));
			parts.SetTo(StringPart(testData[i].path));
			HAM_TEST_ADD_INFO(
				HAM_TEST_EQUAL(parts.Grist().ToStlString(), testData[i].grist)
				HAM_TEST_EQUAL(parts.Root().ToStlString(), testData[i].root)
				HAM_TEST_EQUAL(parts.Directory().ToStlString(),
					testData[i].directory)
				HAM_TEST_EQUAL(parts.BaseName().ToStlString(),
					testData[i].baseName)
				HAM_TEST_EQUAL(parts.Suffix().ToStlString(), testData[i].suffix)
				HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
					testData[i].archiveMember),
				"path: \"%s\"", testData[i].path);
		}
	}
}


void
PathTest::PartsSetters()
{
	{
		Path::Parts parts;
		parts.SetGrist(StringPart("<grist>"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<grist>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.Directory().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(), std::string());
	}

	{
		Path::Parts parts;
		parts.SetRoot(StringPart("root"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("root"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(), std::string());
	}

	{
		Path::Parts parts;
		parts.SetDirectory(StringPart("directory"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("directory"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(), std::string());
	}

	{
		Path::Parts parts;
		parts.SetBaseName(StringPart("base"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.Directory().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("base"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(), std::string());
	}

	{
		Path::Parts parts;
		parts.SetSuffix(StringPart(".suffix"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.Directory().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".suffix"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(), std::string());
	}

	{
		Path::Parts parts;
		parts.SetArchiveMember(StringPart("member.o"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.Directory().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("member.o"));
	}

	{
		Path::Parts parts;
		parts.SetGrist(StringPart("<grist>"));
		parts.SetRoot(StringPart("root"));
		parts.SetDirectory(StringPart("directory"));
		parts.SetBaseName(StringPart("base"));
		parts.SetSuffix(StringPart(".suffix"));
		parts.SetArchiveMember(StringPart("member.o"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<grist>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("root"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("directory"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("base"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".suffix"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("member.o"));

		parts.SetGrist(StringPart("<GrIsT>"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<GrIsT>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("root"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("directory"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("base"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".suffix"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("member.o"));

		parts.SetRoot(StringPart("RoOt"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<GrIsT>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("RoOt"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("directory"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("base"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".suffix"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("member.o"));

		parts.SetDirectory(StringPart("DiReCtOrY"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<GrIsT>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("RoOt"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("DiReCtOrY"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("base"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".suffix"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("member.o"));

		parts.SetBaseName(StringPart("BaSe"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<GrIsT>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("RoOt"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("DiReCtOrY"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("BaSe"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".suffix"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("member.o"));

		parts.SetSuffix(StringPart(".SuFfIx"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<GrIsT>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("RoOt"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("DiReCtOrY"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("BaSe"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".SuFfIx"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("member.o"));

		parts.SetArchiveMember(StringPart("MeMbEr.x"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<GrIsT>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("RoOt"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("DiReCtOrY"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("BaSe"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".SuFfIx"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("MeMbEr.x"));
	}

	{
		Path::Parts parts;
		parts.SetGrist(StringPart("<grist>"));
		parts.SetRoot(StringPart("root"));
		parts.SetDirectory(StringPart("directory"));
		parts.SetBaseName(StringPart("base"));
		parts.SetSuffix(StringPart(".suffix"));
		parts.SetArchiveMember(StringPart("member.o"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<grist>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("root"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("directory"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("base"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".suffix"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("member.o"));

		parts.UnsetGrist();
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("root"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("directory"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("base"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".suffix"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("member.o"));
	}

	{
		Path::Parts parts;
		parts.SetGrist(StringPart("<grist>"));
		parts.SetRoot(StringPart("root"));
		parts.SetDirectory(StringPart("directory"));
		parts.SetBaseName(StringPart("base"));
		parts.SetSuffix(StringPart(".suffix"));
		parts.SetArchiveMember(StringPart("member.o"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<grist>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("root"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("directory"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("base"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".suffix"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("member.o"));

		parts.UnsetRoot();
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<grist>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("directory"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("base"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".suffix"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("member.o"));
	}

	{
		Path::Parts parts;
		parts.SetGrist(StringPart("<grist>"));
		parts.SetRoot(StringPart("root"));
		parts.SetDirectory(StringPart("directory"));
		parts.SetBaseName(StringPart("base"));
		parts.SetSuffix(StringPart(".suffix"));
		parts.SetArchiveMember(StringPart("member.o"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<grist>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("root"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("directory"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("base"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".suffix"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("member.o"));

		parts.UnsetDirectory();
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<grist>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("root"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("base"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".suffix"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("member.o"));
	}

	{
		Path::Parts parts;
		parts.SetGrist(StringPart("<grist>"));
		parts.SetRoot(StringPart("root"));
		parts.SetDirectory(StringPart("directory"));
		parts.SetBaseName(StringPart("base"));
		parts.SetSuffix(StringPart(".suffix"));
		parts.SetArchiveMember(StringPart("member.o"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<grist>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("root"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("directory"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("base"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".suffix"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("member.o"));

		parts.UnsetBaseName();
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<grist>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("root"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("directory"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".suffix"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("member.o"));
	}

	{
		Path::Parts parts;
		parts.SetGrist(StringPart("<grist>"));
		parts.SetRoot(StringPart("root"));
		parts.SetDirectory(StringPart("directory"));
		parts.SetBaseName(StringPart("base"));
		parts.SetSuffix(StringPart(".suffix"));
		parts.SetArchiveMember(StringPart("member.o"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<grist>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("root"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("directory"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("base"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".suffix"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("member.o"));

		parts.UnsetSuffix();
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<grist>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("root"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("directory"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("base"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string());
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("member.o"));
	}

	{
		Path::Parts parts;
		parts.SetGrist(StringPart("<grist>"));
		parts.SetRoot(StringPart("root"));
		parts.SetDirectory(StringPart("directory"));
		parts.SetBaseName(StringPart("base"));
		parts.SetSuffix(StringPart(".suffix"));
		parts.SetArchiveMember(StringPart("member.o"));
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<grist>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("root"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("directory"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("base"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".suffix"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
			std::string("member.o"));

		parts.UnsetArchiveMember();
		HAM_TEST_EQUAL(parts.Grist().ToStlString(), std::string("<grist>"));
		HAM_TEST_EQUAL(parts.Root().ToStlString(), std::string("root"));
		HAM_TEST_EQUAL(parts.Directory().ToStlString(),
			std::string("directory"));
		HAM_TEST_EQUAL(parts.BaseName().ToStlString(), std::string("base"));
		HAM_TEST_EQUAL(parts.Suffix().ToStlString(), std::string(".suffix"));
		HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(), std::string());
	}
}


void
PathTest::PartsPath()
{
	struct TestData {
		const char*	path;
		const char*	grist;
		const char*	root;
		const char*	directory;
		const char*	baseName;
		const char*	suffix;
		const char*	archiveMember;
	};

	const TestData testData[] = {
		{ "", "", "", "", "", "", "" },
		{ "foo", "", "", "", "foo", "", "" },
		{ "bar.a", "", "", "", "bar", ".a", "" },
		{ "foobar.bee", "", "", "", "foobar", ".bee", "" },
		{ "path/foo", "", "", "path", "foo", "", "" },
		{ "path/foo.a", "", "", "path", "foo", ".a", "" },
		{ "some/path/bar.bee", "", "", "some/path", "bar", ".bee", "" },
		{ "/an/absolute/path/foobar.so", "", "", "/an/absolute/path", "foobar",
			".so", "" },
		{ "/", "", "", "/", "", "", "" },
		{ "<grist>", "<grist>", "", "", "", "", "" },
		{ "<>foo", "<>", "", "", "foo", "", "" },
		{ "<g>bar.a", "<g>", "", "", "bar", ".a", "" },
		{ "<gr>foobar.bee", "<gr>", "", "", "foobar", ".bee", "" },
		{ "<gri>path/foo", "<gri>", "", "path", "foo", "", "" },
		{ "<gris>path/foo.a", "<gris>", "", "path", "foo", ".a", "" },
		{ "<grist>some/path/bar.bee", "<grist>", "", "some/path", "bar", ".bee",
			"" },
		{ "<gristt>/an/absolute/path/foobar.so", "<gristt>", "",
			"/an/absolute/path", "foobar", ".so", "" },
		{ "<g>/", "<g>", "", "/", "", "", "" },
		{ "(archive)", "", "", "", "", "", "archive" },
		{ "(member.o)", "", "", "", "", "", "member.o" },
		{ "foo(archive)", "", "", "", "foo", "", "archive" },
		{ "bar.a(archive)", "", "", "", "bar", ".a", "archive" },
		{ "foobar.bee(member.o)", "", "", "", "foobar", ".bee", "member.o" },
		{ "path/foo(archive)", "", "", "path", "foo", "", "archive" },
		{ "path/foo.a(archive)", "", "", "path", "foo", ".a", "archive" },
		{ "some/path/bar.bee(member.o)", "", "", "some/path", "bar", ".bee",
			"member.o" },
		{ "/an/absolute/path/foobar.so(member.o)", "", "", "/an/absolute/path",
			"foobar",".so", "member.o" },
		{ "foo(archive)x", "", "", "", "foo(archive)x", "", "" },
		{ "bar.a(x/archive)", "", "", "bar.a(x", "archive)", "", "" },
		{ "/(archive.o)", "", "", "/", "", "", "archive.o" },
		{ "<>(archive)", "<>", "", "", "", "", "archive" },
		{ "<g>(member.o)", "<g>", "", "", "", "", "member.o" },
		{ "<gr>foo(archive)", "<gr>", "", "", "foo", "", "archive" },
		{ "<gri>bar.a(archive)", "<gri>", "", "", "bar", ".a", "archive" },
		{ "<gris>foobar.bee(member.o)", "<gris>", "", "", "foobar", ".bee",
			"member.o" },
		{ "<grist>path/foo(archive)", "<grist>", "", "path", "foo", "",
			"archive" },
		{ "<>path/foo.a(archive)", "<>", "", "path", "foo", ".a", "archive" },
		{ "<g>some/path/bar.bee(member.o)", "<g>", "", "some/path", "bar",
			".bee", "member.o" },
		{ "<gr>/an/absolute/path/foobar.so(member.o)", "<gr>", "",
			"/an/absolute/path", "foobar",".so", "member.o" },
		{ "<gri>foo(archive)x", "<gri>", "", "", "foo(archive)x", "", "" },
		{ "<gris>bar.a(x/archive)", "<gris>", "", "bar.a(x", "archive)", "",
			"" },
		{ "<grist>/(archive.o)", "<grist>", "", "/", "", "", "archive.o" },
		{ "root/", "", "root", "", "", "", "" },
		{ "root/foo", "", "root", "", "foo", "", "" },
		{ "root/bar.a", "", "root", "", "bar", ".a", "" },
		{ "root/foobar.bee", "", "root", "", "foobar", ".bee", "" },
		{ "root/path/foo", "", "root", "path", "foo", "", "" },
		{ "root/path/foo.a", "", "root", "path", "foo", ".a", "" },
		{ "root/some/path/bar.bee", "", "root", "some/path", "bar", ".bee",
			"" },
		{ "/an/absolute/path/foobar.so", "", "root", "/an/absolute/path",
			"foobar", ".so", "" },
		{ "/", "", "root", "/", "", "", "" },
		{ "<grist>root/", "<grist>", "root", "", "", "", "" },
		{ "<>root/foo", "<>", "root", "", "foo", "", "" },
		{ "<g>root/bar.a", "<g>", "root", "", "bar", ".a", "" },
		{ "<gr>root/foobar.bee", "<gr>", "root", "", "foobar", ".bee", "" },
		{ "<gri>root/path/foo", "<gri>", "root", "path", "foo", "", "" },
		{ "<gris>root/path/foo.a", "<gris>", "root", "path", "foo", ".a", "" },
		{ "<grist>root/some/path/bar.bee", "<grist>", "root", "some/path", "bar",
			".bee", "" },
		{ "<gristt>/an/absolute/path/foobar.so", "<gristt>", "root",
			"/an/absolute/path", "foobar", ".so", "" },
		{ "<g>/", "<g>", "root", "/", "", "", "" },
		{ "root/(archive)", "", "root", "", "", "", "archive" },
		{ "root/(member.o)", "", "root", "", "", "", "member.o" },
		{ "root/foo(archive)", "", "root", "", "foo", "", "archive" },
		{ "root/bar.a(archive)", "", "", "root", "bar", ".a", "archive" },
		{ "root/foobar.bee(member.o)", "", "root", "", "foobar", ".bee",
			"member.o" },
		{ "root/path/foo(archive)", "", "root", "path", "foo", "", "archive" },
		{ "root/path/foo.a(archive)", "", "root", "path", "foo", ".a",
			"archive" },
		{ "root/some/path/bar.bee(member.o)", "", "root", "some/path", "bar",
			".bee", "member.o" },
		{ "/an/absolute/path/foobar.so(member.o)", "", "root",
			"/an/absolute/path", "foobar",".so", "member.o" },
		{ "root/foo(archive)x", "", "root", "", "foo(archive)x", "", "" },
		{ "root/bar.a(x/archive)", "", "root", "bar.a(x", "archive)", "", "" },
		{ "/(archive.o)", "", "root", "/", "", "", "archive.o" },
		{ "<>root/(archive)", "<>", "root", "", "", "", "archive" },
		{ "<g>root/(member.o)", "<g>", "root", "", "", "", "member.o" },
		{ "<gr>root/foo(archive)", "<gr>", "root", "", "foo", "", "archive" },
		{ "<gri>root/bar.a(archive)", "<gri>", "root", "", "bar", ".a",
			"archive" },
		{ "<gris>root/foobar.bee(member.o)", "<gris>", "root", "", "foobar",
			".bee", "member.o" },
		{ "<grist>root/path/foo(archive)", "<grist>", "root", "path", "foo", "",
			"archive" },
		{ "<>root/path/foo.a(archive)", "<>", "root", "path", "foo", ".a",
			"archive" },
		{ "<g>root/some/path/bar.bee(member.o)", "<g>", "root", "some/path",
			"bar", ".bee", "member.o" },
		{ "<gr>/an/absolute/path/foobar.so(member.o)", "<gr>", "root",
			"/an/absolute/path", "foobar",".so", "member.o" },
		{ "<gri>root/foo(archive)x", "<gri>", "root", "", "foo(archive)x", "",
			"" },
		{ "<gris>root/bar.a(x/archive)", "<gris>", "root", "bar.a(x",
			"archive)", "", "" },
		{ "<grist>/(archive.o)", "<grist>", "root", "/", "", "", "archive.o" },
	};

// TODO: Should also test the other behaviors!
	behavior::Behavior behavior(behavior::COMPATIBILITY_HAM);

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		Path::Parts parts;
		parts.SetGrist(StringPart(testData[i].grist));
		parts.SetRoot(StringPart(testData[i].root));
		parts.SetDirectory(StringPart(testData[i].directory));
		parts.SetBaseName(StringPart(testData[i].baseName));
		parts.SetSuffix(StringPart(testData[i].suffix));
		parts.SetArchiveMember(StringPart(testData[i].archiveMember));

		HAM_TEST_ADD_INFO(
			HAM_TEST_EQUAL(parts.Grist().ToStlString(), testData[i].grist)
			HAM_TEST_EQUAL(parts.Root().ToStlString(), testData[i].root)
			HAM_TEST_EQUAL(parts.Directory().ToStlString(),
				testData[i].directory)
			HAM_TEST_EQUAL(parts.BaseName().ToStlString(),
				testData[i].baseName)
			HAM_TEST_EQUAL(parts.Suffix().ToStlString(), testData[i].suffix)
			HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
				testData[i].archiveMember)
			HAM_TEST_EQUAL(parts.ToPath(behavior), testData[i].path),
			"grist: \"%s\", root: \"%s\", directory: \"%s\", baseName: \"%s\", "
			"suffix: \"%s\", archiveMember: \"%s\"", testData[i].grist,
			testData[i].root, testData[i].directory, testData[i].baseName,
			testData[i].suffix, testData[i].archiveMember);

		data::StringBuffer pathBuffer;
		parts.GetPath(pathBuffer, behavior);
		HAM_TEST_ADD_INFO(
			HAM_TEST_EQUAL(std::string(pathBuffer), testData[i].path),
			"grist: \"%s\", root: \"%s\", directory: \"%s\", baseName: \"%s\", "
			"suffix: \"%s\", archiveMember: \"%s\"", testData[i].grist,
			testData[i].root, testData[i].directory, testData[i].baseName,
			testData[i].suffix, testData[i].archiveMember);
	}
}


void
PathTest::PartsIsAbsolute()
{
	struct TestData {
		const char*	grist;
		const char*	root;
		const char*	directory;
		const char*	baseName;
		const char*	suffix;
		const char*	archiveMember;
		bool		isAbsolute;
	};

	const TestData testData[] = {
		{ "", "", "", "", "", "", false },
		{ "", "", "", "foo", "", "", false },
		{ "", "", "", "bar", ".a", "", false },
		{ "", "", "", "foobar", ".bee", "", false },
		{ "", "", "path", "foo", "", "", false },
		{ "", "", "path", "foo", ".a", "", false },
		{ "", "", "some/path", "bar", ".bee", "", false },
		{ "", "", "some/path", "bar", ".bee", "member.o", false },
		{ "", "", "/an/absolute/path", "foobar", ".so", "", true },
		{ "", "", "/path", "foobar", ".so", "member.o", true },
		{ "", "", "/", "", "", "", true },
		{ "", "root", "/", "", "", "", true },
		{ "", "root", "path", "foo", "", "", false },
		{ "", "/root", "path", "foo", "", "", true },
		{ "", "/", "path", "foo", "", "", true },
		{ "<grist>", "", "", "", "", "", false },
		{ "<grist>", "", "", "foo", "", "", false },
		{ "<grist>", "", "", "bar", ".a", "", false },
		{ "<grist>", "", "", "foobar", ".bee", "", false },
		{ "<grist>", "", "path", "foo", "", "", false },
		{ "<grist>", "", "path", "foo", ".a", "", false },
		{ "<grist>", "", "some/path", "bar", ".bee", "", false },
		{ "<grist>", "", "some/path", "bar", ".bee", "member.o", false },
		{ "<grist>", "", "/an/absolute/path", "foobar", ".so", "", true },
		{ "<grist>", "", "/path", "foobar", ".so", "member.o", true },
		{ "<grist>", "", "/", "", "", "", true },
		{ "<grist>", "root", "/", "", "", "", true },
		{ "<grist>", "root", "path", "foo", "", "", false },
		{ "<grist>", "/root", "path", "foo", "", "", true },
		{ "<grist>", "/", "path", "foo", "", "", true },
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		Path::Parts parts;
		parts.SetGrist(StringPart(testData[i].grist));
		parts.SetRoot(StringPart(testData[i].root));
		parts.SetDirectory(StringPart(testData[i].directory));
		parts.SetBaseName(StringPart(testData[i].baseName));
		parts.SetSuffix(StringPart(testData[i].suffix));
		parts.SetArchiveMember(StringPart(testData[i].archiveMember));

		HAM_TEST_ADD_INFO(
			HAM_TEST_EQUAL(parts.Grist().ToStlString(), testData[i].grist)
			HAM_TEST_EQUAL(parts.Root().ToStlString(), testData[i].root)
			HAM_TEST_EQUAL(parts.Directory().ToStlString(),
				testData[i].directory)
			HAM_TEST_EQUAL(parts.BaseName().ToStlString(),
				testData[i].baseName)
			HAM_TEST_EQUAL(parts.Suffix().ToStlString(), testData[i].suffix)
			HAM_TEST_EQUAL(parts.ArchiveMember().ToStlString(),
				testData[i].archiveMember)
			HAM_TEST_EQUAL(parts.IsAbsolute(), testData[i].isAbsolute),
			"grist: \"%s\", root: \"%s\", directory: \"%s\", baseName: \"%s\", "
			"suffix: \"%s\", archiveMember: \"%s\"", testData[i].grist,
			testData[i].root, testData[i].directory, testData[i].baseName,
			testData[i].suffix, testData[i].archiveMember);
	}
}


} // namespace tests
} // namespace ham
