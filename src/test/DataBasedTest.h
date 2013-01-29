/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TEST_DATA_BASED_TEST_H
#define HAM_TEST_DATA_BASED_TEST_H


#include <vector>

#include "test/RunnableTest.h"


namespace ham {
namespace test {


class DataBasedTest : public RunnableTest {
public:
								DataBasedTest(const std::string& name,
									const std::string& code);

			void				AddDataSet(
									const std::vector<std::string>& input,
									const std::vector<std::string>& output,
									uint32_t compatibilityMask);

	virtual	void*				CreateFixture(TestEnvironment* environment);
	virtual	void				DeleteFixture(TestEnvironment* environment,
									void* fixture);
	virtual	uint32_t			TestCaseCompatibility(int index);
	virtual	void				RunTestCase(TestEnvironment* environment,
									void* fixture, int index);

private:
			struct DataSet {
				DataSet(const std::vector<std::string>& input,
					const std::vector<std::string>& output,
					uint32_t compatibilityMask)
					:
					fInput(input),
					fOutput(output),
					fCompatibilityMask(compatibilityMask)
				{
				}

			public:
				std::vector<std::string>	fInput;
				std::vector<std::string>	fOutput;
				uint32_t					fCompatibilityMask;
			};

private:
			void				_RunTest(TestEnvironment* environment,
									const DataSet& dataSet) const;
	static	bool				_ReadEchoLine(TestEnvironment* environment,
									std::istream& input, std::string& _line);

private:
			std::string			fCode;
			std::vector<DataSet> fDataSets;
};


} // namespace test
} // namespace ham


#endif // HAM_TEST_DATA_BASED_TEST_H
