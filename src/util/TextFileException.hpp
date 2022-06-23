/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_UTIL_TEXT_FILE_EXCEPTION_HPP
#define HAM_UTIL_TEXT_FILE_EXCEPTION_HPP

#include "util/Exception.hpp"
#include "util/TextFilePosition.hpp"

#include <string>

namespace ham::util
{

class TextFileException : public Exception
{
  public:
	TextFileException(
		const std::string& message = std::string(),
		const TextFilePosition& position = TextFilePosition()
	)
		: Exception(message),
		  fPosition(position)
	{
	}

	const TextFilePosition& Position() const { return fPosition; }

  private:
	TextFilePosition fPosition;
};

} // namespace ham::util

#endif // HAM_UTIL_TEXT_FILE_EXCEPTION_HPP
