/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TEST_DATA_BASED_TEST_PARSER_HPP
#define HAM_TEST_DATA_BASED_TEST_PARSER_HPP

#include <fstream>

namespace ham::test
{

class Test;

class DataBasedTestParser
{
  public:
	DataBasedTestParser();

	ham::test::Test* Parse(const char* fileName);

  private:
	bool _ReadLine(std::string& _line, std::string& _directive);
	void _Throw(const std::string& message, size_t column = 0);

  private:
	struct TestFile;
	struct TestCase;

  private:
	const char* fFileName;
	std::fstream fInput;
	size_t fLineIndex;
};

} // namespace ham::test

#endif // HAM_TEST_DATA_BASED_TEST_PARSER_HPP
