/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "TestException.h"

#include <stdarg.h>
#include <stdio.h>


using test::TestException;


TestException::TestException(const char* file, int line,
	const char* message,...)
	:
	fFile(file),
	fLine(line)
{
	char buffer[1024];

	va_list args;
	va_start(args, message);
	vsnprintf(buffer, sizeof(buffer), message, args);
	va_end(args);

	fMessage = buffer;
}
