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
	_line.clear();

	bool multiLine = false;
	bool comment = false;
	do {
		comment = false;

		std::string line;
		if (!std::getline(fInput, line)) {
			if (multiLine) {
				_Throw(std::string("Unexpected end of file while reading "
					"multi-line argument"));
			}
			return false;
		}

		fLineIndex++;

		// Comment and directive lines start with '#'. Other lines we copy
		// verbatim.
		if (line[0] != '#') {
			if (!_line.empty())
				_line += '\n';
			_line += line;
			continue;
		}

		// If a non-comment line shall start with "#", it starts with two
		// "#"s instead. So we need to remove one of those.
		if (line[1] == '#') {
			if (!_line.empty())
				_line += '\n';
			_line.append(line.c_str() + 1);
			continue;
		}

		if (line[1] != '!') {
			comment = true;
			continue;
		}

		// It's a directive.
		if (line[2] == '{') {
			if (multiLine)
				_Throw(std::string("Nested multi-line argument not supported"));
			multiLine = true;
		} else if (line[2] == '}') {
			if (!multiLine) {
				_Throw(std::string("Unmatched end of multi-line argument "
					"directive ('#}')"));
			}
			multiLine = false;
		} else
			_Throw(std::string("Unsupported directive \"" + line + "\""));
	} while (multiLine || comment);

	return true;
}


void
DataBasedTestParser::_Throw(const std::string& message, size_t column)
{
	throw parser::ParseException(message,
		parser::ParsePosition(fLineIndex, column));
}


} // namespace test
} // namespace ham
