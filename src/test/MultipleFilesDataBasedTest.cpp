/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "test/MultipleFilesDataBasedTest.h"

#include <sstream>

#include "behavior/Behavior.h"
#include "parser/ParseException.h"
#include "test/TestException.h"
#include "test/TestFixture.h"
#include "util/Constants.h"


namespace ham {
namespace test {


MultipleFilesDataBasedTest::MultipleFilesDataBasedTest(const std::string& name)
	:
	DataBasedTest(name)
{
}


void
MultipleFilesDataBasedTest::AddDataSet(
	const std::map<std::string, std::string>& inputFiles,
	const std::vector<std::string>& output, uint32_t compatibilityMask,
	bool supportedByHam, uint32_t skipMask, size_t startLineIndex,
	size_t endLineIndex)
{
	DataBasedTest::AddDataSet(new DataSet(inputFiles, output, compatibilityMask,
		supportedByHam, skipMask, startLineIndex, endLineIndex));
}


void
MultipleFilesDataBasedTest::PrepareCode(const DataSetBase* dataSetBase,
	const std::string& outputPrefix, const std::string& outputSuffix,
	std::map<std::string, std::string>& _code) const
{
	const DataSet& dataSet = *dynamic_cast<const DataSet*>(dataSetBase);

	_code = dataSet.fInputFiles;

	std::map<std::string, std::string>::iterator it
		= _code.find(util::kJamfileName);
	if (it == _code.end()) {
		HAM_TEST_THROW("No %s among test files\ntest case lines: %zu-%zu",
			util::kJamfileName, dataSet.fStartLineIndex + 1,
			dataSet.fEndLineIndex)
	}

	it->second = outputPrefix + it->second + outputSuffix;
}


} // namespace test
} // namespace ham
