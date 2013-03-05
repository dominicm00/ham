/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "test/DataBasedTestParser.h"

#include <memory>
#include <vector>

#include "parser/ParseException.h"
#include "test/MultipleFilesDataBasedTest.h"
#include "test/TemplateBasedTest.h"


namespace ham {
namespace test {


static const std::string kTestCaseSeparator("---");
static const std::string kInputOutputSeparator("-");
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


static std::string
join_file_content(const std::vector<std::string>& input)
{
	// If the input is code, join all input lines.
	std::string inputCode;
	for (std::vector<std::string>::const_iterator it = input.begin();
		it != input.end(); ++it) {
		inputCode += *it;
		inputCode += '\n';
	}

	return inputCode;
}


struct DataBasedTestParser::TestFile {
public:
	TestFile(const std::vector<std::string>& content, const std::string& file,
		int fileAge, bool joinContent = true)
		:
		fFile(file),
		fAge(fileAge),
		fContent(content),
		fJoinContent(joinContent)
	{
	}

public:
	std::string					fFile;
	int							fAge;
	std::vector<std::string>	fContent;
	bool						fJoinContent;
};


struct DataBasedTestParser::TestCase {
	TestCase(const std::vector<TestFile>& inputFiles,
		const std::vector<TestFile>& outputFiles, bool outputIsException,
		bool earlyExit, uint32_t compatibilityMask, bool supportedByHam,
		uint32_t skipMask, size_t startLineIndex, size_t endLineIndex)
		:
		fCompatibilityMask(compatibilityMask),
		fSupportedByHam(supportedByHam),
		fSkipMask(skipMask),
		fInputFiles(inputFiles),
		fOutputFiles(outputFiles),
		fOutputIsException(outputIsException),
		fEarlyExit(earlyExit),
		fStartLineIndex(startLineIndex),
		fEndLineIndex(endLineIndex)
	{
	}

public:
	uint32_t					fCompatibilityMask;
	bool						fSupportedByHam;
	uint32_t					fSkipMask;
	std::vector<TestFile>		fInputFiles;
	std::vector<TestFile>		fOutputFiles;
	bool						fOutputIsException;
	bool						fEarlyExit;
	size_t						fStartLineIndex;
	size_t						fEndLineIndex;
};


DataBasedTestParser::DataBasedTestParser()
	:
	fInput(),
	fLineIndex(0)
{
}


Test*
DataBasedTestParser::Parse(const char* fileName)
{
	fFileName = fileName;

	fInput.close();
	fInput.open(fileName, std::ios_base::in);
	if (fInput.fail())
		_Throw("Failed to open file");

	fLineIndex = 0;

	// read code
	bool inputIsCode = false;
	bool multipleFiles = false;
	std::string code;
	for (;;) {
		std::string line;
		std::string directive;
		if (!_ReadLine(line, directive)) {
			_Throw(std::string("Unexpected end of file while reading test "
				"code, was expecting separator \"") + kTestCaseSeparator
				+ "\"");
		}

		if (!directive.empty()) {
			if (directive == "inputIsCode") {
				inputIsCode = true;
				continue;
			}

			if (directive == "multipleFiles") {
				multipleFiles = true;
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

	std::vector<TestCase> testCases;

	// read test cases
	std::vector<std::string> previousInput;
	std::vector<std::string> previousOutput;
	bool previousOutputIsException = false;
	bool previousEarlyExit = false;

	bool done = false;

	for (;;) {
		size_t dataSetLineIndex = fLineIndex;
		uint32_t compatibilityMask = behavior::COMPATIBILITY_MASK_ALL;
		uint32_t skipMask = 0;
		bool supportedByHam = true;
		std::vector<TestFile> inputFiles;
		std::vector<std::string> input;
		std::string inputFile;
		int inputFileAge = 0;

		for (;;) {
			std::string line;
			std::string directive;
			if (!_ReadLine(line, directive)) {
				if (input.empty()) {
					done = true;
					break;
				}

				_Throw(std::string("Unexpected end of file while reading test "
					"case input, was expecting separator \"")
					+ kInputOutputSeparator + "\"");
			}

			if (!directive.empty()) {
				if (directive == "repeat") {
					if (input.size() >= previousInput.size()) {
						_Throw(std::string("Repeat directive in test case "
							"input doesn't refer to existing previous input"));
					}

					input.push_back(previousInput.at(input.size()));
					continue;
				}

				if (directive == "compat") {
					compatibilityMask = 0;
					std::vector<std::string> arguments = split_string(line);
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

				if (directive == "skip") {
					skipMask = 0;
					std::vector<std::string> arguments = split_string(line);
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

				if (directive == "file") {
					if (!multipleFiles) {
						_Throw(std::string("\"#!file\" directive requires "
							"#!multipleFiles directive before first test"
							" case"));
					}

					std::vector<std::string> arguments = split_string(line);

					if (arguments.empty() || arguments[0].empty()) {
						_Throw(std::string("\"#!file\" directive requires "
							"file name argument"));
					}

					// commit the previous file
					if (!inputFile.empty()) {
						inputFiles.push_back(
							TestFile(input, inputFile, inputFileAge,
								inputIsCode));
						input.clear();
					}

					inputFile = arguments[0];
					if (arguments.size() > 1)
						inputFileAge = atoi(arguments[1].c_str());
					continue;
				}

				_Throw(std::string("Unsupported directive \"#!" + directive
					+ "\" in test case input"));
			}

			if (line == kInputOutputSeparator) {
				inputFiles.push_back(
					TestFile(input, inputFile, inputFileAge, inputIsCode));
				break;
			}

			if (line == kTestCaseSeparator) {
				_Throw(std::string("Unexpected test case separator \"")
					+ kTestCaseSeparator + "\" while reading test "
					"case input, was expecting separator \""
					+ kInputOutputSeparator + "\"");
			}

			input.push_back(line);
		}

		if (done)
			break;

		std::vector<TestFile> outputFiles;
		std::vector<std::string> output;
		std::string outputFile;
		bool outputIsException = false;
		bool earlyExit = false;
		for (;;) {
			std::string line;
			std::string directive;
			if (!_ReadLine(line, directive)) {
				_Throw(std::string("Unexpected end of file while reading test "
					"case output, was expecting separator \"")
					+ kTestCaseSeparator + "\"");
			}

			if (!directive.empty()) {
				if (directive == "repeat") {
					if (output.size() >= previousOutput.size()) {
						_Throw(std::string("Repeat directive in test case "
							"output doesn't refer to existing previous "
							"output"));
					}

					output.push_back(previousOutput.at(output.size()));
					outputIsException = previousOutputIsException;
					earlyExit = previousEarlyExit;
					continue;
				}

				if (directive == "exception") {
					outputIsException = true;
					continue;
				}

				if (directive == "earlyExit") {
					earlyExit = true;
					continue;
				}

				if (directive == "file") {
					if (!multipleFiles) {
						_Throw(std::string("\"#!file\" directive requires "
							"#!multipleFiles directive before first test "
							"case"));
					}

					if (line.empty()) {
						_Throw(std::string("\"#!file\" directive requires "
							"file name argument"));
					}

					// commit the previous file
					outputFiles.push_back(TestFile(output, outputFile, 0));

					output.clear();
					outputFile = line;
					continue;
				}

				_Throw(std::string("Unsupported directive \"#!" + directive
					+ "\" in test case output"));
			}

			if (line == kTestCaseSeparator) {
				outputFiles.push_back(TestFile(output, outputFile, 0));
				break;
			}

			output.push_back(line);
		}

		previousInput = input;
		previousOutput = output;
		previousOutputIsException = outputIsException;
		previousEarlyExit = earlyExit;

		testCases.push_back(TestCase(inputFiles, outputFiles, outputIsException,
			earlyExit, compatibilityMask & ~skipMask, supportedByHam, skipMask,
			dataSetLineIndex, fLineIndex - 1));
	}

	if (multipleFiles) {
		std::auto_ptr<MultipleFilesDataBasedTest> test(
			new MultipleFilesDataBasedTest(testName));
		for (std::vector<TestCase>::iterator it = testCases.begin();
			it != testCases.end(); ++it) {
			const TestCase& testCase = *it;

			// prepare input files map
			std::map<std::string, std::string> inputFiles;
			std::map<std::string, int> inputFileAges;
			for (std::vector<TestFile>::const_iterator inputFileIt
					= testCase.fInputFiles.begin();
				inputFileIt != testCase.fInputFiles.end(); ++inputFileIt) {
				const TestFile& inputFile = *inputFileIt;
				inputFiles[inputFile.fFile]
					= join_file_content(inputFile.fContent);
				if (inputFile.fAge != 0)
					inputFileAges[inputFile.fFile] = inputFile.fAge;
			}

			// prepare output files map
			std::map<std::string, std::string> outputFiles;
			for (std::vector<TestFile>::const_iterator outputFileIt
					= testCase.fOutputFiles.begin();
				outputFileIt != testCase.fOutputFiles.end(); ++outputFileIt) {
				const TestFile& outputFile = *outputFileIt;
				outputFiles[outputFile.fFile]
					= join_file_content(outputFile.fContent);
			}

			test->AddDataSet(inputFiles, inputFileAges, outputFiles,
				testCase.fOutputIsException, testCase.fEarlyExit,
				testCase.fCompatibilityMask, testCase.fSupportedByHam,
				testCase.fSkipMask, testCase.fStartLineIndex,
				testCase.fEndLineIndex);
		}

		return test.release();
	}

	std::auto_ptr<TemplateBasedTest> test(
		new TemplateBasedTest(testName, code));
	for (std::vector<TestCase>::iterator it = testCases.begin();
		it != testCases.end(); ++it) {
		const TestCase& testCase = *it;

		// prepare output files map
		std::map<std::string, std::string> outputFiles;
		for (std::vector<TestFile>::const_iterator outputFileIt
				= testCase.fOutputFiles.begin();
			outputFileIt != testCase.fOutputFiles.end(); ++outputFileIt) {
			const TestFile& outputFile = *outputFileIt;
			outputFiles[outputFile.fFile]
				= join_file_content(outputFile.fContent);
		}

		std::vector<std::string> content = testCase.fInputFiles.at(0).fContent;
		if (testCase.fInputFiles.at(0).fJoinContent) {
			std::string joinedContent = join_file_content(content);
			content.clear();
			content.push_back(joinedContent);
		}

		test->AddDataSet(content,
			outputFiles, testCase.fOutputIsException,
			testCase.fEarlyExit, testCase.fCompatibilityMask,
			testCase.fSupportedByHam, testCase.fSkipMask,
			testCase.fStartLineIndex, testCase.fEndLineIndex);
	}

	return test.release();
}


bool
DataBasedTestParser::_ReadLine(std::string& _line, std::string& _directive)
{
	_line.clear();
	_directive.clear();

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
			size_t directiveEnd = line.find_first_of(' ', 2);
			if (directiveEnd == std::string::npos)
				directiveEnd = line.length();
			if (directiveEnd == 2)
				_Throw(std::string("Invalid directive line: \"" + line + "\""));

			_directive = std::string(line, 2, directiveEnd - 2);

			size_t argumentsStart = line.find_first_not_of(' ', directiveEnd);
			if (argumentsStart == std::string::npos)
				argumentsStart = line.length();

			_line.append(line.c_str() + argumentsStart);
		}
	} while (multiLine || comment);

	return true;
}


void
DataBasedTestParser::_Throw(const std::string& message, size_t column)
{
	throw parser::ParseException(message, fFileName,
		parser::ParsePosition(fLineIndex, column));
}


} // namespace test
} // namespace ham
