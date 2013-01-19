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
									const std::vector<std::string>& output);

	virtual	void*				CreateFixture(TestEnvironment* environment);
	virtual	void				DeleteFixture(TestEnvironment* environment,
									void* fixture);
	virtual	void				RunTestCase(TestEnvironment* environment,
									void* fixture, int index);

private:
			struct DataSet {
				DataSet(const std::vector<std::string>& input,
					const std::vector<std::string>& output)
					:
					fInput(input),
					fOutput(output)
				{
				}

			public:
				std::vector<std::string> fInput;
				std::vector<std::string> fOutput;
			};

private:
			void				_RunTest(TestEnvironment* environment,
									const DataSet& dataSet) const;

private:
			std::string			fCode;
			std::vector<DataSet> fDataSets;
};


} // namespace test
} // namespace ham


#endif // HAM_TEST_DATA_BASED_TEST_H
