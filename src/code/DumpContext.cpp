/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/DumpContext.h"

#include <iostream>


using code::DumpContext;

DumpContext::DumpContext()
	:
	fNodeLevel(0),
	fNewLine(true)
{
}


DumpContext&
DumpContext::PrintString(const std::string& string)
{
	size_t pos = string.length() > 0 ? 0 : std::string::npos;

	while (pos != std::string::npos && pos < string.length()) {
		if (fNewLine)
			std::cout << std::string(fNodeLevel * 2, ' ');

		size_t newLinePos = string.find('\n', pos);
		fNewLine = newLinePos != std::string::npos;

		if (fNewLine) {
			// also dump the newline
			newLinePos++;
		}

		std::cout << std::string(string, pos, newLinePos);

		pos = newLinePos;
	}

	return *this;
}
