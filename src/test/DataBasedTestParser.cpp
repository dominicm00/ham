/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "test/DataBasedTestParser.h"

#include <memory>
#include <vector>

#include "parser/ParseException.h"
#include "test/TemplateBasedTest.h"


namespace ham {
namespace test {


static const std::string kTestCaseSeparator("---");
static const std::string kInputOutputSeparator("-");
static const std::string kCompatibilityDirective("compat ");
static const std::string kSkipDirective("skip ");
static const std::string kVersionJam("jam");
static const std::string kVersionBoostJam("boost");
static const std::string kVersionHam("ham");
static const std::string kCompatibilityNotHam("!ham");


static std::vector<std::string>
split_string(const std::string& string)
{
	std::vector<std::string> result;

	std::string remainder(string);
	size_t index = 0;
	while ((index = remainder.find_first_not_of(' ', index))
			!= std::string::npos) {
		size_t end = remainder.find(' ', index);
		std::string substring(remainder, index,
			end != std::string::npos ? end : remainder.length() - index);
		result.push_back(substring);
		index += substring.length();
	}

	return result;
}


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
	bool inputIsCode = false;
	std::string code;
	for (;;) {
		std::string line;
		bool directive;
		if (!_ReadLine(line, directive)) {
			_Throw(std::string("Unexpected end of file while reading test "
				"code, was expecting separator \"") + kTestCaseSeparator
				+ "\"");
		}

		if (directive) {
			if (line == "inputIsCode") {
				inputIsCode = true;
				continue;
			}

			_Throw(std::string("Unsupported directive \"#!" + line + "\" in "
				"test code"));
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

	std::auto_ptr<TemplateBasedTest> test(
		new TemplateBasedTest(testName, code));

	// read test cases
	std::vector<std::string> previousInput;
	std::vector<std::string> previousOutput;

	for (;;) {
		size_t dataSetLineIndex = fLineIndex;
		uint32_t compatibilityMask = behavior::COMPATIBILITY_MASK_ALL;
		uint32_t skipMask = 0;
		bool supportedByHam = true;
		std::vector<std::string> input;
		for (;;) {
			std::string line;
			bool directive;
			if (!_ReadLine(line, directive)) {
				if (input.empty())
					return test.release();

				_Throw(std::string("Unexpected end of file while reading test "
					"case input, was expecting separator \"")
					+ kInputOutputSeparator + "\"");
			}

			if (directive) {
				if (line == "repeat") {
					if (input.size() >= previousInput.size()) {
						_Throw(std::string("Repeat directive in test case "
							"input doesn't refer to existing previous input"));
					}

					input.push_back(previousInput.at(input.size()));
					continue;
				}

				if (line.compare(0, kCompatibilityDirective.length(),
						kCompatibilityDirective) == 0) {
					compatibilityMask = 0;
					std::vector<std::string> arguments = split_string(
						std::string(line, kCompatibilityDirective.length()));
					for (std::vector<std::string>::iterator it
							= arguments.begin();
						it != arguments.end(); ++it) {
						std::string versionString = *it;
						if (versionString == kVersionJam) {
							compatibilityMask
								|= 1 << behavior::COMPATIBILITY_JAM;
						} else if (versionString == kVersionBoostJam) {
							compatibilityMask
								|= 1 << behavior::COMPATIBILITY_BOOST_JAM;
						} else if (versionString == kVersionHam) {
							compatibilityMask
								|= 1 << behavior::COMPATIBILITY_HAM;
						} else if (versionString == kCompatibilityNotHam) {
							supportedByHam = false;
						} else {
							_Throw(std::string("Invalid argument for "
								"\"#!compat\" directive: \"" + versionString
								+ "\""));
						}
					}
					continue;
				}

				if (line.compare(0, kSkipDirective.length(), kSkipDirective)
						== 0) {
					skipMask = 0;
					std::vector<std::string> arguments = split_string(
						std::string(line, kSkipDirective.length()));
					for (std::vector<std::string>::iterator it
							= arguments.begin();
						it != arguments.end(); ++it) {
						std::string versionString = *it;
						if (versionString == kVersionJam) {
							skipMask |= 1 << behavior::COMPATIBILITY_JAM;
						} else if (versionString == kVersionBoostJam) {
							skipMask |= 1 << behavior::COMPATIBILITY_BOOST_JAM;
						} else if (versionString == kVersionHam) {
							skipMask |= 1 << behavior::COMPATIBILITY_HAM;
						} else {
							_Throw(std::string("Invalid argument for "
								"\"#!skip\" directive: \"" + versionString
								+ "\""));
						}
					}
					continue;
				}

				_Throw(std::string("Unsupported directive \"#!" + line
					+ "\" in test case input"));
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

		// If the input is code, join all input lines.
		if (inputIsCode) {
			std::string inputCode;
			for (std::vector<std::string>::iterator it = input.begin();
				it != input.end(); ++it) {
				if (it != input.begin())
					inputCode += '\n';
				inputCode += *it;
			}

			input.clear();
			input.push_back(inputCode);
		}

		std::vector<std::string> output;
		for (;;) {
			std::string line;
			bool directive;
			if (!_ReadLine(line, directive)) {
				_Throw(std::string("Unexpected end of file while reading test "
					"case output, was expecting separator \"")
					+ kTestCaseSeparator + "\"");
			}

			if (directive) {
				if (line == "repeat") {
					if (output.size() >= previousOutput.size()) {
						_Throw(std::string("Repeat directive in test case "
							"output doesn't refer to existing previous "
							"output"));
					}

					output.push_back(previousOutput.at(output.size()));
					continue;
				} else {
					_Throw(std::string("Unsupported directive \"#!" + line
						+ "\" in test case output"));
				}
			}

			if (line == kTestCaseSeparator)
				break;

			output.push_back(line);
		}

		previousInput = input;
		previousOutput = output;
		test->AddDataSet(input, output, compatibilityMask & ~skipMask,
			supportedByHam, skipMask, dataSetLineIndex, fLineIndex - 1);
	}
}


bool
DataBasedTestParser::_ReadLine(std::string& _line, bool& _directive)
{
	_line.clear();
	_directive = false;

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
		} else {
			_directive = true;
			_line.append(line.c_str() + 2);
		}
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
