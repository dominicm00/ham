/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "test/DataBasedTestParser.h"

#include <memory>
#include <vector>

#include "parser/ParseException.h"
#include "test/DataBasedTest.h"


namespace ham {
namespace test {


static const std::string kTestCaseSeparator("---");
static const std::string kInputOutputSeparator("-");


DataBasedTestParser::DataBasedTestParser()
	:
	fInput(),
	fLineIndex(0)
{
}


Test*
DataBasedTestParser::Parse(const char* fileName)
{
	fInput.close();
	fInput.open(fileName, std::ios_base::in);
	if (fInput.fail())
		_Throw("Failed to open file");

	fLineIndex = 0;

	// read code
	std::string code;
	for (;;) {
		std::string line;
		if (!_ReadLine(line)) {
			_Throw(std::string("Unexpected end of file while reading test "
				"code, was expecting separator \"") + kTestCaseSeparator
				+ "\"");
		}

		if (line == kTestCaseSeparator)
			break;

		code += line;
		code += '\n';
	}

	std::string testName(fileName);
	size_t testNameSlash = testName.rfind('/');
	if (testNameSlash != std::string::npos)
		testName.erase(0, testNameSlash + 1);

	std::auto_ptr<DataBasedTest> test(new DataBasedTest(testName, code));

	// read test cases
	for (;;) {
		std::vector<std::string> input;
		for (;;) {
			std::string line;
			if (!_ReadLine(line)) {
				if (input.empty())
					return test.release();

				_Throw(std::string("Unexpected end of file while reading test "
					"case input, was expecting separator \"")
					+ kInputOutputSeparator + "\"");
			}

			if (line == kInputOutputSeparator)
				break;

			if (line == kTestCaseSeparator) {
				_Throw(std::string("Unexpected test case separator \"")
					+ kTestCaseSeparator + "\" while reading test "
					"case input, was expecting separator \""
					+ kInputOutputSeparator + "\"");
			}

			input.push_back(line);
		}

		std::vector<std::string> output;
		for (;;) {
			std::string line;
			if (!_ReadLine(line)) {
				_Throw(std::string("Unexpected end of file while reading test "
					"case output, was expecting separator \"")
					+ kTestCaseSeparator + "\"");
			}

			if (line == kTestCaseSeparator)
				break;

			output.push_back(line);
		}

		test->AddDataSet(input, output);
	}
}


bool
DataBasedTestParser::_ReadLine(std::string& _line)
{
	for (;;) {
		if (!std::getline(fInput, _line))
			return false;

		fLineIndex++;

		// skip comment lines
		if (_line[0] != '#')
			return true;

		// If a non-comment line shall starting with "#", it starts with two
		// "#"s instead. So we need to remove one of those.
		if (_line[1] == '#') {
			_line.erase(0, 1);
			return true;
		}
	}
}


void
DataBasedTestParser::_Throw(const std::string& message, size_t column)
{
	throw parser::ParseException(message,
		parser::ParsePosition(fLineIndex, column));
}


} // namespace test
} // namespace ham
