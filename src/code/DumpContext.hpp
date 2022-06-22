/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_DUMP_CONTEXT_HPP
#define HAM_CODE_DUMP_CONTEXT_HPP

#include <sstream>

#include "data/String.hpp"

namespace ham::code
{

class DumpContext
{
  public:
	DumpContext();

	inline void BeginChildren();
	inline void EndChildren();

	template<typename Type>
	inline DumpContext& operator<<(const Type& value);

	DumpContext& PrintString(const std::string& string);

  private:
	int fNodeLevel;
	bool fNewLine;
};

void
DumpContext::BeginChildren()
{
	fNodeLevel++;
}

void
DumpContext::EndChildren()
{
	fNodeLevel--;
}

template<typename Type>
DumpContext&
DumpContext::operator<<(const Type& value)
{
	// At the beginning of each line, we insert spaces for indentation. So
	// we print the value to a string stream and analyze the string.
	std::stringstream stream;
	stream << value;

	return PrintString(stream.str());
}

} // namespace ham::code

#endif // HAM_CODE_DUMP_CONTEXT_HPP
