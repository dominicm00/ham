/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_UTIL_EXCEPTION_HPP
#define HAM_UTIL_EXCEPTION_HPP

#include <string>

namespace ham::util
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

} // namespace ham::util

#endif // HAM_UTIL_EXCEPTION_HPP
