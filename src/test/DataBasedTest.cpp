/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "test/DataBasedTest.h"

#include <sstream>

#include "behavior/Behavior.h"
#include "util/TextFileException.h"
#include "test/TestException.h"
#include "test/TestFixture.h"


namespace ham {
namespace test {


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
	PrepareCode(dataSet, std::string("Echo ") + kOutputPrefix + " ;\n",
		std::string("Echo ") + kOutputSuffix + " ;\n", code);

	// execute the code
	std::stringstream outputStream;
	try {
		TestFixture::ExecuteCode(environment, code, outputStream, outputStream);
	} catch (util::TextFileException& exception) {
		if (dataSet->fOutputIsException) {
			// and check the exception message against what's expected
			std::vector<std::string> output;
			output.push_back(exception.Message());
			HAM_TEST_ADD_INFO(
				HAM_TEST_EQUAL(output, dataSet->fOutput),
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
	}

	if (dataSet->fOutputIsException) {
		HAM_TEST_THROW("Expected exception: \"%s\"\nActual output: \"%s\"\n"
			"code:\n%s\nlines: %zu-%zu",
			TestFixture::ValueToString(dataSet->fOutput).c_str(),
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
		HAM_TEST_ADD_INFO(
			HAM_TEST_VERIFY(_ReadEchoLine(environment, outputStream, line)),
			"output: \"%s\"\nlines: %zu-%zu", outputStream.str().c_str(),
			dataSet->fStartLineIndex + 1, dataSet->fEndLineIndex)
		if (line == kOutputSuffix)
			break;
		output.push_back(line);
	}

	// and check the output against what's expected
	HAM_TEST_ADD_INFO(
		HAM_TEST_EQUAL(output, dataSet->fOutput),
			"code:\n%s\nlines: %zu-%zu", _CodeToString(code).c_str(),
			dataSet->fStartLineIndex + 1, dataSet->fEndLineIndex)
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
