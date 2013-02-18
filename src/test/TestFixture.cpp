/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "test/TestFixture.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <iterator>
#include <sstream>

#include "code/Processor.h"
#include "test/TestEnvironment.h"
#include "util/Constants.h"


namespace ham {
namespace test {


template<typename Container>
static std::string
value_container_to_string(const Container& value)
{
	std::ostringstream stream;
	stream << "{";

	for (typename Container::const_iterator it = value.begin();
			it != value.end(); ++it) {
		stream << ' ' << TestFixture::ValueToString(*it);
	}

	stream << " }";
	return stream.str();
}


// #pragma mark - TemporaryDirectoryCreator


TestFixture::TemporaryDirectoryCreator::TemporaryDirectoryCreator()
{
}


TestFixture::TemporaryDirectoryCreator::~TemporaryDirectoryCreator()
{
	Delete();
}


const char*
TestFixture::TemporaryDirectoryCreator::Create(bool changeDirectory)
{
	Delete();

	try {
		// get the current working directory
		if (changeDirectory)
			fOldWorkingDirectory = CurrentWorkingDirectory();

		// create a temporary test directory
		CreateTemporaryDirectory(fTemporaryDirectory);

		// change the directory to the test directory
		if (changeDirectory && chdir(fTemporaryDirectory.c_str()) < 0) {
			fOldWorkingDirectory.clear();
				// clear, since we don't need to chdir() back
			HAM_TEST_THROW("Failed to cd into temporary directory: %s",
				strerror(errno))
		}
	} catch (...) {
		Delete();
		throw;
	}

	return fTemporaryDirectory.c_str();
}


void
TestFixture::TemporaryDirectoryCreator::Delete()
{
	// change the directory back to the original directory
	if (!fOldWorkingDirectory.empty()) {
		chdir(fOldWorkingDirectory.c_str());
		fOldWorkingDirectory.clear();
	}

	// remove the temporary directory
	if (!fTemporaryDirectory.empty()) {
		std::string directory = fTemporaryDirectory;
		fTemporaryDirectory.clear();
		RemoveRecursively(directory);
	}
}


// #pragma mark - ExecutableExecuter


struct TestFixture::CodeExecuter {
	void Execute(const char* jamExecutable,
		behavior::Compatibility compatibility,
		const std::map<std::string, std::string>& code, std::ostream& output,
		std::ostream& errorOutput)
	{
		fTemporaryDirectoryCreator.Delete();
		fOutputPipe = NULL;

		try {
			// create a temporary test directory and change into it
			fTemporaryDirectoryCreator.Create(true);

			// write the code to the Jamfiles
			for (std::map<std::string, std::string>::const_iterator it
					= code.begin();
				it != code.end(); ++it) {
				std::string jamfilePath = MakePath(
					fTemporaryDirectoryCreator.Directory(), it->first.c_str());
				CreateFile(jamfilePath.c_str(), it->second.c_str());
			}

			if (jamExecutable != NULL) {
				// run the executable
				fOutputPipe = popen(jamExecutable, "r");
				if (fOutputPipe == NULL) {
					HAM_TEST_THROW("Failed to execute \"%s\": %s",
						jamExecutable, strerror(errno))
				}

				// read input until done
				std::ostream_iterator<char> outputIterator(output);
				char buffer[4096];
				for (;;) {
					size_t bytesRead = fread(buffer, 1, sizeof(buffer),
						fOutputPipe);
					if (bytesRead > 0) {
						outputIterator = std::copy(buffer, buffer + bytesRead,
							outputIterator);
					}

					if (bytesRead < sizeof(buffer))
						break;
				}
			} else {
				code::Processor processor;
				processor.SetCompatibility(compatibility);
				processor.SetOutput(output);
				processor.SetErrorOutput(errorOutput);
				processor.ProcessJambase();
			}
		} catch (...) {
			_Cleanup();
			throw;
		}

		_Cleanup();
	}

private:
	void _Cleanup()
	{
		// close pipe to jam process
		if (fOutputPipe != NULL)
			pclose(fOutputPipe);

		// change the directory back to the original directory and delete the
		// temporary directory
		fTemporaryDirectoryCreator.Delete();
	}

private:
	TemporaryDirectoryCreator	fTemporaryDirectoryCreator;
	FILE*						fOutputPipe;
};


// #pragma mark - TestFixture


/*static*/ data::StringList
TestFixture::MakeStringList(const char* element1, const char* element2,
	const char* element3, const char* element4, const char* element5,
	const char* element6, const char* element7, const char* element8,
	const char* element9, const char* element10, const char* element11,
	const char* element12, const char* element13, const char* element14,
	const char* element15, const char* element16, const char* element17,
	const char* element18, const char* element19, const char* element20)
{
	const char* const elements[] = {
		element1, element2, element3, element4, element5, element6, element7,
		element8, element9, element10, element11, element12, element13,
		element14, element15, element16, element17, element18, element19,
		element20
	};

	data::StringList list;
	for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
		if (elements[i] == NULL)
			break;
		list.Append(data::String(elements[i]));
	}

	return list;
}


/*static*/ data::StringList
TestFixture::MakeStringList(const std::vector<std::string>& testList)
{
	StringList list;
	for (std::vector<std::string>::const_iterator it = testList.begin();
			it != testList.end(); ++it) {
		list.Append(it->c_str());
	}

	return list;
}

/*static*/ data::StringListList
TestFixture::MakeStringListList(
	const std::vector<std::vector<std::string> >& testListList)
{
	StringListList listList;
	for (std::vector<std::vector<std::string> >::const_iterator it
			= testListList.begin(); it != testListList.end(); ++it) {
		listList.push_back(MakeStringList(*it));
	}

	return listList;
}


/*static*/ void
TestFixture::ExecuteCode(TestEnvironment* environment, const std::string& code,
	std::ostream& output, std::ostream& errorOutput)
{
	std::string jamExecutable = environment->JamExecutable();
	std::map<std::string,std::string> codeFiles;
	codeFiles[util::kJamfileName] = code;
	return ExecuteCode(environment, codeFiles, output, errorOutput);
}


/*static*/ void
TestFixture::ExecuteCode(TestEnvironment* environment,
	const std::map<std::string, std::string>& code, std::ostream& output,
	std::ostream& errorOutput)
{
	std::string jamExecutable = environment->JamExecutable();
	return CodeExecuter().Execute(
		jamExecutable.empty() ? NULL : jamExecutable.c_str(),
		environment->GetCompatibility(), code, output, errorOutput);
}


/*static */ std::string
TestFixture::CurrentWorkingDirectory()
{
	long size = pathconf(".", _PC_PATH_MAX);
	char* buffer = new char[size];

	try {
		if (getcwd(buffer, (size_t)size) == NULL) {
			HAM_TEST_THROW("Failed to get current working directory: %s",
				strerror(errno))
		}

		std::string result(buffer);
		delete[] buffer;
		buffer = NULL;
		return result;
	} catch (...) {
		delete[] buffer;
		throw;
	}
}


/*static*/ void
TestFixture::CreateTemporaryDirectory(std::string& _path)
{
	// get a name for the temporary directory
	char temporaryDirectoryBuffer[L_tmpnam + 1];
	char* temporaryDirectory = tmpnam(temporaryDirectoryBuffer);
	if (temporaryDirectory == NULL)
		HAM_TEST_THROW("tmpnam() didn't return new temporary file name.")

	_path = temporaryDirectory;

	// create it
	CreateDirectory(temporaryDirectory, false);
}


/*static*/ void
TestFixture::CreateParentDirectory(const char* path, bool createAncestors)
{
// TODO: Path delimiter!
	const char* lastSlash = strrchr(path, '/');
// TODO: Platform specific (root path).
	if (lastSlash == NULL || lastSlash == path)
		return;

	std::string parentPath(path, lastSlash - path);
	CreateDirectory(parentPath.c_str(), createAncestors);
}


/*static*/ void
TestFixture::CreateDirectory(const char* path, bool createAncestors)
{
// TODO: Platform specific.
	struct stat st;
	if (lstat(path, &st) == 0) {
		// already exists?
		if (S_ISDIR(st.st_mode))
			return;

		HAM_TEST_THROW("Can't create directory \"%s\", a file is in the way",
			path)
	}

	if (createAncestors)
		CreateParentDirectory(path, true);

	if (mkdir(path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) < 0) {
		HAM_TEST_THROW("Failed to create directory \"%s\": %s", path,
			strerror(errno))
	}
}

void TestFixture::CreateFile(const char* path, const char* content)
{
	CreateParentDirectory(path);
// TODO: Replace path delimiters with platform specific ones!
	std::fstream file(path, std::ios_base::out);
	if (file.fail()) {
		HAM_TEST_THROW("Failed to create temporary file \"%s\".", path)
	}

	std::copy(content, content + strlen(content),
		std::ostream_iterator<char>(file));

	if (file.fail()) {
		HAM_TEST_THROW("Failed to write content to temporary file \"%s\".",
			path)
	}
}


/*static*/ void
TestFixture::RemoveRecursively(std::string entry)
{
	struct stat st;
	if (lstat(entry.c_str(), &st) < 0) {
		HAM_TEST_THROW("Failed to stat entry \"%s\" for removal: %s",
			strerror(errno))
	}

	if (S_ISDIR(st.st_mode)) {
		// recursively remove all entries in the directory
		DIR* dir = opendir(entry.c_str());
		if (dir == NULL) {
			HAM_TEST_THROW("Failed to open directory \"%s\" for removal: %s",
				entry.c_str(), strerror(errno))
		}

		try {
			while (struct dirent* dirEntry = readdir(dir)) {
				if (strcmp(dirEntry->d_name, ".") == 0
					|| strcmp(dirEntry->d_name, "..") == 0) {
					continue;
				}

				RemoveRecursively(MakePath(entry.c_str(), dirEntry->d_name));
			}
		} catch (...) {
			closedir(dir);
			throw;
		}

		closedir(dir);

		// remove the directory itself
		if (rmdir(entry.c_str()) < 0) {
			HAM_TEST_THROW("Failed to remove directory \"%s\": %s",
				entry.c_str(), strerror(errno))
		}
	} else {
		if (unlink(entry.c_str()) < 0) {
			HAM_TEST_THROW("Failed to remove entry \"%s\": %s", entry.c_str(),
				strerror(errno))
		}
	}
}


/*static*/ std::string
TestFixture::MakePath(const char* head, const char* tail)
{
	return std::string(head) + '/' + tail;
// TODO: Path delimiter!
}


#define DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(type)	\
	template<>												\
	std::string												\
	TestFixture::ValueToString<type>(const type& value)		\
	{														\
		std::ostringstream stream;							\
		stream << value;									\
		return stream.str();								\
	}


DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(char)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(short)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(int)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(long)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(long long)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(unsigned char)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(unsigned short)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(unsigned int)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(unsigned long)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(unsigned long long)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(bool)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(float)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(double)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(long double)


template<>
/*static*/ std::string
TestFixture::ValueToString<const char*>(const char* const& value)
{
	return std::string("'") + value + "'";
}


template<>
/*static*/ std::string
TestFixture::ValueToString<std::string>(const std::string& value)
{
	return ValueToString(value.c_str());
}


template<>
/*static*/ std::string
TestFixture::ValueToString<std::list<std::string> >(
	const std::list<std::string>& value)
{
	return value_container_to_string(value);
}


template<>
/*static*/ std::string
TestFixture::ValueToString<std::vector<std::string> >(
	const std::vector<std::string>& value)
{
	return value_container_to_string(value);
}


template<>
/*static*/ std::string
TestFixture::ValueToString<data::StringPart>(const data::StringPart& value)
{
	return TestFixture::ValueToString(value.ToStlString());
}


template<>
/*static*/ std::string
TestFixture::ValueToString<data::String>(const data::String& value)
{
	return TestFixture::ValueToString(value.ToCString());
}


template<>
/*static*/ std::string
TestFixture::ValueToString<data::StringList>(const data::StringList& value)
{
	std::ostringstream stream;
	stream << "{";

	for (StringList::Iterator it = value.GetIterator(); it.HasNext();)
		stream << ' ' << ValueToString(it.Next());

	stream << " }";
	return stream.str();
}


template<>
/*static*/ std::string
TestFixture::ValueToString<data::StringListList>(
	const data::StringListList& value)
{
	return value_container_to_string(value);
}


template<>
/*static*/ std::string
TestFixture::ValueToString<std::pair<size_t, size_t> >(
	const std::pair<size_t, size_t>& value)
{
	std::ostringstream stream;
	stream << "(" << value.first << ", " << value.second << ")";
	return stream.str();
}


template<>
/*static*/ std::string
TestFixture::ValueToString<std::vector<std::pair<size_t, size_t> > >(
	const std::vector<std::pair<size_t, size_t> >& value)
{
	return value_container_to_string(value);
}


} // namespace test
} // namespace ham
