/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TEST_TEMPLATE_BASED_TEST_H
#define HAM_TEST_TEMPLATE_BASED_TEST_H


#include "test/DataBasedTest.h"


namespace ham {
namespace test {


class TemplateBasedTest : public DataBasedTest {
public:
								TemplateBasedTest(const std::string& name,
									const std::string& code);

			void				AddDataSet(
									const std::vector<std::string>& input,
									const std::vector<std::string>& output,
									uint32_t compatibilityMask,
									bool supportedByHam, uint32_t skipMask,
									size_t startLineIndex, size_t endLineIndex);

protected:
	virtual	void				PrepareCode(const DataSetBase* dataSet,
									const std::string& outputPrefix,
									const std::string& outputSuffix,
									std::map<std::string, std::string>& _code)
									const;

private:
			struct DataSet : public DataSetBase {
				DataSet(const std::vector<std::string>& input,
					const std::vector<std::string>& output,
					uint32_t compatibilityMask, bool supportedByHam,
					uint32_t skipMask, size_t startLineIndex,
					size_t endLineIndex)
					:
					DataSetBase(output, compatibilityMask, supportedByHam,
						skipMask, startLineIndex, endLineIndex),
					fInput(input)
				{
				}

			public:
				std::vector<std::string>	fInput;
			};

private:
			std::string			fCode;
};


} // namespace test
} // namespace ham


#endif // HAM_TEST_TEMPLATE_BASED_TEST_H
