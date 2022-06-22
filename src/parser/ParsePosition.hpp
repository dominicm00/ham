/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_PARSER_PARSE_POSITION_HPP
#define HAM_PARSER_PARSE_POSITION_HPP

#include <stddef.h>

namespace ham
{
namespace parser
{

struct ParsePosition {
	ParsePosition()
		: fLine(0),
		  fColumn(0)
	{
	}

	ParsePosition(size_t line, size_t column)
		: fLine(line),
		  fColumn(column)
	{
	}

	void SetTo(size_t line, size_t column)
	{
		fLine = line;
		fColumn = column;
	}

	size_t Line() const { return fLine; }

	size_t Column() const { return fColumn; }

  private:
	size_t fLine;
	size_t fColumn;
};

} // namespace parser
} // namespace ham

#endif // HAM_PARSER_PARSE_POSITION_HPP
