/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TEST_DATA_BASED_TEST_H
#define HAM_TEST_DATA_BASED_TEST_H

#include <map>
#include <set>
#include <vector>

#include "test/RunnableTest.h"

namespace ham
{
namespace test
{

class DataBasedTest : public RunnableTest
{
  public:
	enum ExitState {
		EXIT_OK,
		EXIT_EVALUATION_ERROR,
		EXIT_BIND_ERROR,
		EXIT_MAKE_ERROR
	};

  public:
	DataBasedTest(const std::string& name);
	virtual ~DataBasedTest();

	virtual void* CreateFixture(TestEnvironment* environment);
	virtual void DeleteFixture(TestEnvironment* environment, void* fixture);
	virtual uint32_t TestCaseCompatibility(int index,
										   bool& _supportedByHam,
										   uint32_t& _skipMask);
	virtual void RunTestCase(TestEnvironment* environment,
							 void* fixture,
							 int index);

  protected:
	struct DataSetBase {
		DataSetBase(const std::map<std::string, std::string>& outputFiles,
					const std::set<std::string>& missingOutputFiles,
					bool outputIsException,
					ExitState exitState,
					uint32_t compatibilityMask,
					bool supportedByHam,
					uint32_t skipMask,
					size_t startLineIndex,
					size_t endLineIndex)
			: fOutputFiles(outputFiles),
			  fMissingOutputFiles(missingOutputFiles),
			  fOutputIsException(outputIsException),
			  fExitState(exitState),
			  fCompatibilityMask(compatibilityMask),
			  fSupportedByHam(supportedByHam),
			  fSkipMask(skipMask),
			  fStartLineIndex(startLineIndex),
			  fEndLineIndex(endLineIndex)
		{
		}

		virtual ~DataSetBase() {}

	  public:
		std::map<std::string, std::string> fOutputFiles;
		std::set<std::string> fMissingOutputFiles;
		bool fOutputIsException;
		ExitState fExitState;
		uint32_t fCompatibilityMask;
		bool fSupportedByHam;
		uint32_t fSkipMask;
		size_t fStartLineIndex;
		size_t fEndLineIndex;
	};

  protected:
	void AddDataSet(DataSetBase* dataSet);

	virtual void PrepareCode(const DataSetBase* dataSet,
							 const std::string& outputPrefix,
							 const std::string& outputSuffix,
							 std::map<std::string, std::string>& _code,
							 std::map<std::string, int>& _codeAge) const = 0;

  private:
	void _RunTest(TestEnvironment* environment,
				  const DataSetBase* dataSet) const;
	static bool _ReadEchoLine(TestEnvironment* environment,
							  std::istream& input,
							  std::string& _line);
	static std::string _CodeToString(
		const std::map<std::string, std::string>& code);

  private:
	std::vector<DataSetBase*> fDataSets;
};

} // namespace test
} // namespace ham

#endif // HAM_TEST_DATA_BASED_TEST_H
