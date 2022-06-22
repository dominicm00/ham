/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "test/MultipleFilesDataBasedTest.hpp"

#include <sstream>

#include "behavior/Behavior.hpp"
#include "parser/ParseException.hpp"
#include "test/TestException.hpp"
#include "test/TestFixture.hpp"
#include "util/Constants.hpp"

namespace ham::test
{

MultipleFilesDataBasedTest::MultipleFilesDataBasedTest(const std::string& name)
	: DataBasedTest(name)
{
}

void
MultipleFilesDataBasedTest::AddDataSet(
	const std::map<std::string, std::string>& inputFiles,
	const std::map<std::string, int>& inputFileAges,
	const std::map<std::string, std::string>& outputFiles,
	const std::set<std::string>& missingOutputFiles,
	bool outputIsException,
	ExitState exitState,
	uint32_t compatibilityMask,
	bool supportedByHam,
	uint32_t skipMask,
	size_t startLineIndex,
	size_t endLineIndex)
{
	DataBasedTest::AddDataSet(new DataSet(inputFiles,
										  inputFileAges,
										  outputFiles,
										  missingOutputFiles,
										  outputIsException,
										  exitState,
										  compatibilityMask,
										  supportedByHam,
										  skipMask,
										  startLineIndex,
										  endLineIndex));
}

void
MultipleFilesDataBasedTest::PrepareCode(
	const DataSetBase* dataSetBase,
	const std::string& outputPrefix,
	const std::string& outputSuffix,
	std::map<std::string, std::string>& _code,
	std::map<std::string, int>& _codeAge) const
{
	const DataSet& dataSet = *dynamic_cast<const DataSet*>(dataSetBase);

	_code = dataSet.fInputFiles;
	_codeAge = dataSet.fInputFileAges;

	std::map<std::string, std::string>::iterator it =
		_code.find(util::kJamfileName);
	if (it == _code.end()) {
		HAM_TEST_THROW("No %s among test files\ntest case lines: %zu-%zu",
					   util::kJamfileName,
					   dataSet.fStartLineIndex + 1,
					   dataSet.fEndLineIndex)
	}

	it->second = outputPrefix + it->second + outputSuffix;
}

} // namespace ham::test
