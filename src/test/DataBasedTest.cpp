/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "test/DataBasedTest.h"

#include <fstream>
#include <sstream>

#include "behavior/Behavior.h"
#include "util/TextFileException.h"
#include "test/TestException.h"
#include "test/TestFixture.h"


namespace ham {
namespace test {


static std::vector<std::string>
split_lines(const std::string& string)
{
	std::vector<std::string> lines;
	size_t index = 0;
	for (;;) {
		size_t lineEnd = string.find_first_of('\n', index);
		if (lineEnd == std::string::npos) {
			// All lines are supposed to end with '\n'.
			break;
		}

		lines.push_back(std::string(string, index, lineEnd - index));
		index = lineEnd + 1;
	}

	return lines;
}


DataBasedTest::DataBasedTest(const std::string& name)
	:
	RunnableTest(name, true)
{
}


DataBasedTest::~DataBasedTest()
{
	for (std::vector<DataSetBase*>::iterator it = fDataSets.begin();
		it != fDataSets.end(); ++it) {
		delete *it;
	}
}


void*
DataBasedTest::CreateFixture(TestEnvironment* environment)
{
	// dummy fixture
	return this;
}


void
DataBasedTest::DeleteFixture(TestEnvironment* environment, void* fixture)
{
}


uint32_t
DataBasedTest::TestCaseCompatibility(int index, bool& _supportedByHam,
	uint32_t& _skipMask)
{
	const DataSetBase* dataSet = fDataSets[index];
	_supportedByHam = dataSet->fSupportedByHam;
	_skipMask = dataSet->fSkipMask;
	return dataSet->fCompatibilityMask;
}


void
DataBasedTest::RunTestCase(TestEnvironment* environment, void* fixture,
	int index)
{
	_RunTest(environment, fDataSets[index]);
}


void
DataBasedTest::AddDataSet(DataSetBase* dataSet)
{
	try {
		fDataSets.push_back(dataSet);
	} catch (...) {
		delete dataSet;
		throw;
	}

	std::stringstream testCaseName;
	testCaseName << fDataSets.size();
	fTestCaseNames.push_back(testCaseName.str());
}


void
DataBasedTest::_RunTest(TestEnvironment* environment,
	const DataSetBase* dataSet) const
{
	static const char* const kOutputPrefix = "---test-output-start---";
	static const char* const kOutputSuffix = "---test-output-end---";

	std::map<std::string, std::string> code;
	std::map<std::string, int> codeAge;
	PrepareCode(dataSet, std::string("Echo ") + kOutputPrefix + " ;\n",
		std::string("Echo ") + kOutputSuffix + " ;\n", code, codeAge);

	// execute the code
	std::string expectedOutput = dataSet->fOutputFiles.at("");
	std::stringstream outputStream;
	TestFixture::CodeExecuter codeExecuter;
	try {
		codeExecuter.Execute(environment, code, codeAge, outputStream,
			outputStream);
	} catch (util::TextFileException& exception) {
		if (dataSet->fOutputIsException) {
			// and check the exception message against what's expected
			std::string expectedExceptionMessage
				= split_lines(expectedOutput).at(0);
			HAM_TEST_ADD_INFO(
				HAM_TEST_EQUAL(exception.Message(), expectedExceptionMessage),
					"code:\n%s\nlines: %zu-%zu", _CodeToString(code).c_str(),
					dataSet->fStartLineIndex + 1, dataSet->fEndLineIndex)
			return;
		}

		HAM_TEST_THROW(
			"%s.\nat %zu:%zu of file \"%s\":\n%s\ntest case lines: %zu-%zu",
			exception.Message(), exception.Position().Line() + 1,
			exception.Position().Column() + 1, exception.Position().FileName(),
			_CodeToString(code).c_str(), dataSet->fStartLineIndex + 1,
			dataSet->fEndLineIndex)
	} catch (util::Exception& exception) {
		if (dataSet->fOutputIsException) {
			// and check the exception message against what's expected
			std::string expectedExceptionMessage
				= split_lines(expectedOutput).at(0);
			HAM_TEST_ADD_INFO(
				HAM_TEST_EQUAL(exception.Message(), expectedExceptionMessage),
					"code:\n%s\nlines: %zu-%zu", _CodeToString(code).c_str(),
					dataSet->fStartLineIndex + 1, dataSet->fEndLineIndex)
			return;
		}

		HAM_TEST_THROW(
			"%s.\n%s\ntest case lines: %zu-%zu", exception.Message(),
			_CodeToString(code).c_str(), dataSet->fStartLineIndex + 1,
			dataSet->fEndLineIndex)
	}

	if (dataSet->fOutputIsException) {
		std::string expectedExceptionMessage
			= split_lines(expectedOutput).at(0);
		HAM_TEST_THROW("Expected exception: \"%s\"\nActual output: \"%s\"\n"
			"code:\n%s\nlines: %zu-%zu", expectedExceptionMessage.c_str(),
			outputStream.str().c_str(), _CodeToString(code).c_str(),
			dataSet->fStartLineIndex + 1, dataSet->fEndLineIndex)
	}

	// extract the output
	outputStream.seekg(0, std::ios_base::beg);
	std::string line;
	for (;;) {
		HAM_TEST_ADD_INFO(
			HAM_TEST_VERIFY(_ReadEchoLine(environment, outputStream, line)),
			"output: \"%s\"\nlines: %zu-%zu", outputStream.str().c_str(),
			dataSet->fStartLineIndex + 1, dataSet->fEndLineIndex)
		if (line == kOutputPrefix)
			break;
	}

	std::vector<std::string> output;
	for (;;) {
		if (!_ReadEchoLine(environment, outputStream, line)) {
			if (dataSet->fEarlyExit)
				break;
			HAM_TEST_THROW("Unexpected end of output (looking for end of "
				"output marker).\noutput: \"%s\"\nlines: %zu-%zu",
				outputStream.str().c_str(), dataSet->fStartLineIndex + 1,
				dataSet->fEndLineIndex)
		}
		if (line == kOutputSuffix)
			break;
		output.push_back(line);
	}

	// and check the output against what's expected
	HAM_TEST_ADD_INFO(
		HAM_TEST_EQUAL(output, split_lines(expectedOutput)),
		"code:\n%s\nlines: %zu-%zu", _CodeToString(code).c_str(),
		dataSet->fStartLineIndex + 1, dataSet->fEndLineIndex)

	// check the expected output files
	for (std::map<std::string, std::string>::const_iterator it
			= dataSet->fOutputFiles.begin();
		it != dataSet->fOutputFiles.end(); ++it) {
		std::string fileName = it->first;
		if (fileName.empty())
			continue;

		std::ifstream file(fileName);
		HAM_TEST_ADD_INFO(
			HAM_TEST_VERIFY(!file.fail()),
			"output file: %s\ncode:\n%s\nlines: %zu-%zu", fileName.c_str(),
			_CodeToString(code).c_str(), dataSet->fStartLineIndex + 1,
			dataSet->fEndLineIndex)

		std::string content;
		std::string line;
		while (std::getline(file, line)) {
			content += line;
			content += '\n';
		}

		HAM_TEST_ADD_INFO(
			HAM_TEST_EQUAL(content, it->second),
			"output file: %s\ncode:\n%s\nlines: %zu-%zu", fileName.c_str(),
			_CodeToString(code).c_str(), dataSet->fStartLineIndex + 1,
			dataSet->fEndLineIndex)
	}
}


/*static*/ bool
DataBasedTest::_ReadEchoLine(TestEnvironment* environment,
	std::istream& input, std::string& _line)
{
	if (!std::getline(input, _line))
		return false;

	// Jam prints a space at the end of each line printed via Echo. Chop it off
	// to get comparable results.
	behavior::Behavior behavior(environment->GetCompatibility());
	if (behavior.GetEchoTrailingSpace()
			== behavior::Behavior::ECHO_TRAILING_SPACE) {
		size_t length = _line.length();
		if (length > 0 && _line[length - 1] == ' ')
			_line.resize(length - 1);
	}

	return true;
}


std::string
DataBasedTest::_CodeToString(const std::map<std::string, std::string>& code)
{
	std::string string;
	for (std::map<std::string, std::string>::const_iterator it = code.begin();
		it != code.end(); ++it) {
		string += "[";
		string += it->first;
		string += "]\n";
		string += it->second;
	}
	return string;
}


} // namespace test
} // namespace ham
