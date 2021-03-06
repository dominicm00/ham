/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TEST_TEMPLATE_BASED_TEST_HPP
#define HAM_TEST_TEMPLATE_BASED_TEST_HPP

#include "test/DataBasedTest.hpp"

namespace ham::test
{

class TemplateBasedTest : public DataBasedTest
{
  public:
	TemplateBasedTest(const std::string& name, const std::string& code);

	void AddDataSet(
		const std::vector<std::string>& input,
		const std::map<std::string, std::string>& outputFiles,
		const std::set<std::string>& missingOutputFiles,
		bool outputIsException,
		ExitState exitState,
		uint32_t compatibilityMask,
		bool supportedByHam,
		uint32_t skipMask,
		size_t startLineIndex,
		size_t endLineIndex
	);

  protected:
	virtual void PrepareCode(
		const DataSetBase* dataSet,
		const std::string& outputPrefix,
		const std::string& outputSuffix,
		std::map<std::string, std::string>& _code,
		std::map<std::string, int>& _codeAge
	) const;

  private:
	class DataSet : public DataSetBase
	{
	  public:
		DataSet(
			const std::vector<std::string>& input,
			const std::map<std::string, std::string>& outputFiles,
			const std::set<std::string>& missingOutputFiles,
			bool outputIsException,
			ExitState exitState,
			uint32_t compatibilityMask,
			bool supportedByHam,
			uint32_t skipMask,
			size_t startLineIndex,
			size_t endLineIndex
		)
			: DataSetBase(
				outputFiles,
				missingOutputFiles,
				outputIsException,
				exitState,
				compatibilityMask,
				supportedByHam,
				skipMask,
				startLineIndex,
				endLineIndex
			),
			  fInput(input)
		{
		}

	  public:
		std::vector<std::string> fInput;
	};

  private:
	std::string fCode;
};

} // namespace ham::test

#endif // HAM_TEST_TEMPLATE_BASED_TEST_HPP
