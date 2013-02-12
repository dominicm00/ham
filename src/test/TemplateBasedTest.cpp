/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "test/TemplateBasedTest.h"

#include <sstream>

#include "behavior/Behavior.h"
#include "parser/ParseException.h"
#include "test/TestException.h"
#include "test/TestFixture.h"


namespace ham {
namespace test {


TemplateBasedTest::TemplateBasedTest(const std::string& name,
	const std::string& code)
	:
	RunnableTest(name, true),
	fCode(code)
{
}


void
TemplateBasedTest::AddDataSet(const std::vector<std::string>& input,
	const std::vector<std::string>& output, uint32_t compatibilityMask,
	bool supportedByHam, uint32_t skipMask, size_t startLineIndex,
	size_t endLineIndex)
{
	fDataSets.push_back(
		DataSet(input, output, compatibilityMask, supportedByHam, skipMask,
			startLineIndex, endLineIndex));

	std::stringstream testCaseName;
	testCaseName << fDataSets.size();
	fTestCaseNames.push_back(testCaseName.str());
}


void*
TemplateBasedTest::CreateFixture(TestEnvironment* environment)
{
	// dummy fixture
	return this;
}


void
TemplateBasedTest::DeleteFixture(TestEnvironment* environment, void* fixture)
{
}


uint32_t
TemplateBasedTest::TestCaseCompatibility(int index, bool& _supportedByHam,
	uint32_t& _skipMask)
{
	const DataSet& dataSet = fDataSets[index];
	_supportedByHam = dataSet.fSupportedByHam;
	_skipMask = dataSet.fSkipMask;
	return dataSet.fCompatibilityMask;
}


void
TemplateBasedTest::RunTestCase(TestEnvironment* environment, void* fixture,
	int index)
{
	_RunTest(environment, fDataSets[index]);
}


void
TemplateBasedTest::_RunTest(TestEnvironment* environment,
	const DataSet& dataSet) const
{
	static const char* const kOutputPrefix = "---test-output-start---";
	static const char* const kOutputSuffix = "---test-output-end---";

	// Prepend code to output a marker. Later we append code to output another
	// marker, so we can easily get the actual test output.
	std::string code = std::string("Echo ") + kOutputPrefix + " ;\n";

	// substitute input data into the code
	const char* codeTemplate = fCode.c_str();
	const char* codeEnd = codeTemplate + fCode.length();
	const char* verbatim = codeTemplate;
	for (;;) {
		const char* percent = std::find(codeTemplate, codeEnd, '%');
		if (percent == codeEnd) {
			if (verbatim != codeEnd)
				code += std::string(verbatim, codeEnd - verbatim);
			break;
		}

		if (percent != verbatim)
			code += std::string(verbatim, percent - verbatim);

		percent++;
		if (*percent == '%') {
			verbatim = percent;
			codeTemplate = verbatim + 1;
			continue;
		}

		char* indexEnd;
		long index = strtol(percent, &indexEnd, 10);
		if (indexEnd == percent) {
			throw TestException(__FILE__, __LINE__,
				"Stray '%%' in code template:\n%s", fCode.c_str());
		}

		if (index < 1 || (size_t)index > dataSet.fInput.size()) {
			throw TestException(__FILE__, __LINE__,
				"Code template requires input data element at %ld, but got "
				"only %zu input elements (lines %zu-%zu).", index,
				dataSet.fInput.size(), dataSet.fStartLineIndex + 1,
				dataSet.fEndLineIndex);
		}

		code += dataSet.fInput[index - 1];
		codeTemplate = verbatim = indexEnd;
	}

	// Append code to output the end marker.
	code += std::string("Echo ") + kOutputSuffix + " ;\n";

	// execute the code
	std::stringstream outputStream;
	try {
		TestFixture::ExecuteCode(environment, code, outputStream, outputStream);
	} catch (parser::ParseException& exception) {
		HAM_TEST_THROW(
			"%s.\nat %zu:%zu of code:\n%s\ntest case lines: %zu-%zu",
			exception.Message(), exception.Position().Line() + 1,
			exception.Position().Column() + 1, code.c_str(),
			dataSet.fStartLineIndex + 1, dataSet.fEndLineIndex)
	}
	// extract the output
	outputStream.seekg(0, std::ios_base::beg);
	std::string line;
	for (;;) {
		HAM_TEST_ADD_INFO(
			HAM_TEST_VERIFY(_ReadEchoLine(environment, outputStream, line)),
			"output: \"%s\"\nlines: %zu-%zu", outputStream.str().c_str(),
			dataSet.fStartLineIndex + 1, dataSet.fEndLineIndex)
		if (line == kOutputPrefix)
			break;
	}

	std::vector<std::string> output;
	for (;;) {
		HAM_TEST_ADD_INFO(
			HAM_TEST_VERIFY(_ReadEchoLine(environment, outputStream, line)),
			"output: \"%s\"\nlines: %zu-%zu", outputStream.str().c_str(),
			dataSet.fStartLineIndex + 1, dataSet.fEndLineIndex)
		if (line == kOutputSuffix)
			break;
		output.push_back(line);
	}

	// and check the output against what's expected
	HAM_TEST_ADD_INFO(
		HAM_TEST_EQUAL(output, dataSet.fOutput),
			"code:\n%s\nlines: %zu-%zu", code.c_str(),
			dataSet.fStartLineIndex + 1, dataSet.fEndLineIndex)
}


/*static*/ bool
TemplateBasedTest::_ReadEchoLine(TestEnvironment* environment,
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


} // namespace test
} // namespace ham
