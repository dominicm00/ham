/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "test/TemplateBasedTest.hpp"

#include "behavior/Behavior.hpp"
#include "parser/ParseException.hpp"
#include "test/TestException.hpp"
#include "test/TestFixture.hpp"
#include "util/Constants.hpp"

#include <sstream>

namespace ham::test
{

TemplateBasedTest::TemplateBasedTest(const std::string& name,
									 const std::string& code)
	: DataBasedTest(name),
	  fCode(code)
{
}

void
TemplateBasedTest::AddDataSet(
	const std::vector<std::string>& input,
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
	DataBasedTest::AddDataSet(new DataSet(input,
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
TemplateBasedTest::PrepareCode(const DataSetBase* dataSetBase,
							   const std::string& outputPrefix,
							   const std::string& outputSuffix,
							   std::map<std::string, std::string>& _code,
							   std::map<std::string, int>& _codeAge) const
{
	const DataSet& dataSet = *dynamic_cast<const DataSet*>(dataSetBase);

	// Prepend code to output a marker. Later we append code to output another
	// marker, so we can easily get the actual test output.
	std::string code = outputPrefix;

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
			throw TestException(__FILE__,
								__LINE__,
								"Stray '%%' in code template:\n%s",
								fCode.c_str());
		}

		if (index < 1 || (size_t)index > dataSet.fInput.size()) {
			throw TestException(
				__FILE__,
				__LINE__,
				"Code template requires input data element at %ld, but got "
				"only %zu input elements (lines %zu-%zu).",
				index,
				dataSet.fInput.size(),
				dataSet.fStartLineIndex + 1,
				dataSet.fEndLineIndex);
		}

		code += dataSet.fInput[index - 1];
		codeTemplate = verbatim = indexEnd;
	}

	// Append code to output the end marker.
	code += outputSuffix;

	_code.clear();
	_code[util::kJamfileName] = code;
	_codeAge.clear();
}

} // namespace ham::test
