/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "test/DataBasedTest.h"

#include <sstream>

#include "test/TestException.h"
#include "test/TestFixture.h"


namespace ham {
namespace test {


DataBasedTest::DataBasedTest(const std::string& name, const std::string& code)
	:
	RunnableTest(name, true),
	fCode(code)
{
	fTestCaseNames.push_back("");
}


void
DataBasedTest::AddDataSet(const std::vector<std::string>& input,
	const std::vector<std::string>& output)
{
	fDataSets.push_back(DataSet(input, output));
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


void
DataBasedTest::RunTestCase(TestEnvironment* environment, void* fixture,
	int index)
{
	size_t dataSetCount = fDataSets.size();
	for (size_t i = 0; i < dataSetCount; i++)
		_RunTest(environment, fDataSets[i]);
}


void
DataBasedTest::_RunTest(TestEnvironment* environment,
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
				"only %zu input elements.", index, dataSet.fInput.size());
		}

		code += dataSet.fInput[index - 1];
		codeTemplate = verbatim = indexEnd;
	}

	// Append code to output the end marker.
	code += std::string("Echo ") + kOutputSuffix + " ;\n";

	// execute the code
	std::stringstream outputStream;
	TestFixture::ExecuteCode(environment, code, outputStream, outputStream);

	// extract the output
	outputStream.seekg(0, std::ios_base::beg);
	std::string line;
	for (;;) {
		HAM_TEST_ADD_INFO(
			HAM_TEST_VERIFY(std::getline(outputStream, line)),
			"output: \"%s\"", outputStream.str().c_str())
		if (line == kOutputPrefix)
			break;
	}

	std::vector<std::string> output;
	for (;;) {
		HAM_TEST_ADD_INFO(
			HAM_TEST_VERIFY(std::getline(outputStream, line)),
			"output: \"%s\"", outputStream.str().c_str())
		if (line == kOutputSuffix)
			break;
		output.push_back(line);
	}

	// and check the output against what's expected
	HAM_TEST_ADD_INFO(
		HAM_TEST_EQUAL(output, dataSet.fOutput),
		"code:\n%s", code.c_str())
}


} // namespace test
} // namespace ham
