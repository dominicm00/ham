/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TEST_MULTIPLE_FILES_DATA_BASED_TEST_HPP
#define HAM_TEST_MULTIPLE_FILES_DATA_BASED_TEST_HPP

#include "test/DataBasedTest.hpp"

#include <map>

namespace ham::test
{

class MultipleFilesDataBasedTest : public DataBasedTest
{
  public:
	MultipleFilesDataBasedTest(const std::string& name);

	void AddDataSet(
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
			  fInputFiles(inputFiles),
			  fInputFileAges(inputFileAges)
		{
		}

	  public:
		std::map<std::string, std::string> fInputFiles;
		std::map<std::string, int> fInputFileAges;
	};
};

} // namespace ham::test

#endif // HAM_TEST_MULTIPLE_FILES_DATA_BASED_TEST_HPP
