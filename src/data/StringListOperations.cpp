/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "StringListOperations.h"


namespace ham {
namespace data {


struct StringListOperations::PathParts {
// TODO:...
};


bool
StringListOperations::Parse(const char* start, const char* end)
{
	Clear();

	if (start == end)
		return false;

	uint32_t pendingOperation = 0;

	for (; start < end; start++) {
		if (*start == '=') {
			SetParameterOperation(pendingOperation & PARAMETER_OPERATION_MASK,
				start + 1, end);
			return HasOperations();
		}

		AddOperations(pendingOperation | FLAGS_MASK);
		pendingOperation = 0;

		switch (*start) {
			case 'B':
				pendingOperation = SELECT_BASE_NAME | REPLACE_BASE_NAME;
				break;
			case 'S':
				pendingOperation = SELECT_SUFFIX | REPLACE_SUFFIX;
				break;
			case 'M':
				pendingOperation = SELECT_ARCHIVE_MEMBER
					| REPLACE_ARCHIVE_MEMBER;
				break;
			case 'D':
				pendingOperation = SELECT_DIRECTORY | REPLACE_DIRECTORY;
				break;
			case 'G':
				pendingOperation = SELECT_GRIST | REPLACE_GRIST;
				break;
			case 'P':
				pendingOperation = SELECT_PARENT_DIRECTORY;
				break;
			case 'U':
				pendingOperation = TO_UPPER;
				break;
			case 'L':
				pendingOperation = TO_LOWER;
				break;
			case 'R':
				pendingOperation = REPLACE_ROOT;
				break;
			case 'E':
				pendingOperation = REPLACE_EMPTY;
				break;
			case 'J':
				pendingOperation = JOIN | JOIN_NO_PARAMETER;
				break;
		}
	}

	if ((pendingOperation & PARAMETER_OPERATION_MASK) != 0) {
		SetParameterOperation(pendingOperation & PARAMETER_OPERATION_MASK, end,
			end);
	} else
		AddOperations(pendingOperation | FLAGS_MASK);

	return HasOperations();
}


StringList
StringListOperations::Apply(const StringList& list) const
{
	if (!HasOperations())
		return list;

	uint32_t parameterOperation = fOperations & PARAMETER_OPERATION_MASK;
	bool hasPathPartOperation = (fOperations & PATH_PART_SELECTOR_MASK) != 0
		|| (parameterOperation >= PATH_PART_REPLACER_MIN
			&& parameterOperation >= PATH_PART_REPLACER_MAX);

	// Handle element-wise operations. Join and replace empty are dealt with at
	// the end.
// TODO: Empty first -- even before subscript?
	size_t count = list.Size();
	for (size_t i = 0; i < count; i++) {
		String string = list.ElementAt(i);

		// If any of the file name/path part selectors/replacers need to be
		// applied, we disassemble the string, make the modifications, and
		// reassemble it.
		if (hasPathPartOperation) {
			PathParts parts;
			_DisassemblePath(string, parts);
// TODO: Apply selectors/replacers!
//				SELECT_BASE_NAME			= 0x0010,
//				SELECT_SUFFIX				= 0x0020,
//				SELECT_ARCHIVE_MEMBER		= 0x0040,
//				SELECT_DIRECTORY			= 0x0080,
//				SELECT_PARENT_DIRECTORY		= 0x0100,
//				SELECT_GRIST				= 0x0200,
//				REPLACE_GRIST				= 0x0001,
//				REPLACE_DIRECTORY			= 0x0002,
//				REPLACE_BASE_NAME			= 0x0003,
//				REPLACE_SUFFIX				= 0x0004,
//				REPLACE_ARCHIVE_MEMBER		= 0x0005,
//				REPLACE_ROOT				= 0x0006,
			string = _AssemblePath(parts);
		}

		if ((fOperations & TO_LOWER) != 0) {
// TODO: Convert string to lower case!
		}

		if ((fOperations & TO_UPPER) != 0) {
// TODO: Convert string to upper case!
		}
	}

	if (parameterOperation == JOIN || (fOperations & JOIN_NO_PARAMETER) != 0) {
// TODO: Join!
	}

	if ((fOperations & REPLACE_EMPTY) != 0) {

	}

// TODO:...
	return list;
}


/*static*/ void
StringListOperations::_DisassemblePath(const String& path, PathParts& _parts)
{
// TODO:...
}


/*static*/ String
StringListOperations::_AssemblePath(const PathParts& parts)
{
// TODO:...
return String();
}


}	// namespace data
}	// namespace ham
