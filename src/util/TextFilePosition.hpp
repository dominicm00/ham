/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_UTIL_TEXT_FILE_POSITION_HPP
#define HAM_UTIL_TEXT_FILE_POSITION_HPP

#include <string>

namespace ham::util
{

struct TextFilePosition {
	TextFilePosition()
		: fLine(0),
		  fColumn(0)
	{
	}

	TextFilePosition(const std::string& fileName, size_t line, size_t column)
		: fFileName(fileName),
		  fLine(line),
		  fColumn(column)
	{
	}

	void SetTo(const std::string& fileName, size_t line, size_t column)
	{
		fFileName = fileName;
		fLine = line;
		fColumn = column;
	}

	const char* FileName() const { return fFileName.c_str(); }

	size_t Line() const { return fLine; }

	size_t Column() const { return fColumn; }

  private:
	std::string fFileName;
	size_t fLine;
	size_t fColumn;
};

} // namespace ham::util

#endif // HAM_UTIL_TEXT_FILE_POSITION_HPP
