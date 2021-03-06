/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TEST_TEST_EXCEPTION_HPP
#define HAM_TEST_TEST_EXCEPTION_HPP

#include <string>

namespace ham::test
{

class TestException
{
  public:
	TestException(const char* file, int line, const char* message, ...);

	const char* File() const { return fFile; }
	int Line() const { return fLine; }
	const std::string& Message() const { return fMessage; }

	void ThrowWithExtendedMessage(const char* message, ...);

  private:
	const char* fFile;
	int fLine;
	std::string fMessage;
};

} // namespace ham::test

#endif // HAM_TEST_TEST_EXCEPTION_HPP
