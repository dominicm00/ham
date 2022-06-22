/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_MAKE_MAKE_EXCEPTION_HPP
#define HAM_MAKE_MAKE_EXCEPTION_HPP

#include "util/Exception.hpp"

namespace ham::make
{

class MakeException : public util::Exception
{
  public:
	MakeException(const std::string& message = std::string())
		: util::Exception(message)
	{
	}
};

} // namespace ham::make

#endif // HAM_MAKE_MAKE_EXCEPTION_HPP
