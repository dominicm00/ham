/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_UTIL_EXCEPTION_H
#define HAM_UTIL_EXCEPTION_H

#include <string>

namespace ham
{
namespace util
{

class Exception
{
  public:
	Exception(const std::string& message = std::string())
		: fMessage(message)
	{
	}

	const char* Message() const { return fMessage.c_str(); }

  private:
	std::string fMessage;
};

} // namespace util
} // namespace ham

#endif // HAM_UTIL_EXCEPTION_H
