/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "TestException.hpp"

#include <stdarg.h>
#include <stdio.h>

namespace ham::test
{

TestException::TestException(
	const char* file,
	int line,
	const char* message,
	...
)
	: fFile(file),
	  fLine(line)
{
	char buffer[4096];

	va_list args;
	va_start(args, message);
	vsnprintf(buffer, sizeof(buffer), message, args);
	va_end(args);

	fMessage = buffer;
}

void
TestException::ThrowWithExtendedMessage(const char* message, ...)
{
	char buffer[4096];

	va_list args;
	va_start(args, message);
	vsnprintf(buffer, sizeof(buffer), message, args);
	va_end(args);

	throw TestException(fFile, fLine, "%s\n%s", fMessage.c_str(), buffer);
}

} // namespace ham::test
