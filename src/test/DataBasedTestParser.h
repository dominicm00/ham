/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TEST_DATA_BASED_TEST_PARSER_H
#define HAM_TEST_DATA_BASED_TEST_PARSER_H


#include <fstream>


namespace ham {
namespace test {


class Test;


class DataBasedTestParser {
public:
								DataBasedTestParser();

			ham::test::Test*	Parse(const char* fileName);

private:
			bool				_ReadLine(std::string& _line, bool& _directive);
			void				_Throw(const std::string& message,
									size_t column = 0);

private:
			std::fstream		fInput;
			size_t				fLineIndex;
};


} // namespace test
} // namespace ham


#endif // HAM_TEST_DATA_BASED_TEST_PARSER_H
