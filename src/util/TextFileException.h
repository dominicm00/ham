/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_UTIL_TEXT_FILE_EXCEPTION_H
#define HAM_UTIL_TEXT_FILE_EXCEPTION_H


#include <string>

#include "util/Exception.h"
#include "util/TextFilePosition.h"


namespace ham {
namespace util {


class TextFileException : public Exception {
public:
	TextFileException(const std::string& message = std::string(),
		const TextFilePosition& position = TextFilePosition())
		:
		Exception(message),
		fPosition(position)
	{
	}

	const TextFilePosition& Position() const
	{
		return fPosition;
	}

private:
	TextFilePosition	fPosition;
};


}	// namespace util
}	// namespace ham


#endif	// HAM_UTIL_TEXT_FILE_EXCEPTION_H
