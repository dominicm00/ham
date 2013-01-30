/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "data/StringListOperations.h"


namespace ham {
namespace data {


// #pragma mark - PathParts


struct StringListOperations::PathParts {
	StringPart	fGrist;
	StringPart	fRoot;
	StringPart	fDirectory;
	StringPart	fBaseName;
	StringPart	fSuffix;
	StringPart	fArchiveMember;
};


// #pragma mark - StringListOperations


StringListOperations::StringListOperations()
	:
	fOperations(0),
	fGristParameter(),
	fDirectoryParameter(),
	fBaseNameParameter(),
	fSuffixParameter(),
	fArchiveMemberParameter(),
	fRootParameter(),
	fEmptyParameter(),
	fJoinParameter()
{
}


void
StringListOperations::Parse(const char* start, const char* end)
{
	uint32_t pendingOperation = 0;
	StringPart* pendingParameter = NULL;

	for (; start < end; start++) {
		if (*start == '=') {
			if (pendingParameter != NULL) {
				pendingParameter->SetTo(start + 1, end);
				AddOperations(pendingOperation & PARAMETER_OPERATION_MASK);
			}
			return;
		}

		AddOperations(pendingOperation & NO_PARAMETER_OPERATION_MASK);
		pendingOperation = 0;
		pendingParameter = NULL;

		switch (*start) {
			case 'G':
				pendingOperation = SELECT_GRIST | REPLACE_GRIST;
				pendingParameter = &fGristParameter;
				break;
			case 'R':
				pendingOperation = SELECT_ROOT | REPLACE_ROOT;
				pendingParameter = &fRootParameter;
				break;
			case 'P':
				pendingOperation = TO_PARENT_DIRECTORY;
				break;
			case 'D':
				pendingOperation = SELECT_DIRECTORY | REPLACE_DIRECTORY;
				pendingParameter = &fDirectoryParameter;
				break;
			case 'B':
				pendingOperation = SELECT_BASE_NAME | REPLACE_BASE_NAME;
				pendingParameter = &fBaseNameParameter;
				break;
			case 'S':
				pendingOperation = SELECT_SUFFIX | REPLACE_SUFFIX;
				pendingParameter = &fSuffixParameter;
				break;
			case 'M':
				pendingOperation = SELECT_ARCHIVE_MEMBER
					| REPLACE_ARCHIVE_MEMBER;
				pendingParameter = &fArchiveMemberParameter;
				break;
			case 'U':
				pendingOperation = TO_UPPER;
				break;
			case 'L':
				pendingOperation = TO_LOWER;
				break;
			case 'E':
				pendingOperation = REPLACE_EMPTY;
				pendingParameter = &fEmptyParameter;
				break;
			case 'J':
				pendingOperation = JOIN;
				pendingParameter = &fJoinParameter;
				break;
		}
	}

	if ((pendingOperation & NO_PARAMETER_OPERATION_MASK) != 0) {
		AddOperations(pendingOperation & NO_PARAMETER_OPERATION_MASK);
	} else if (pendingParameter != NULL) {
		pendingParameter->SetTo(end, end);
		AddOperations(pendingOperation & PARAMETER_OPERATION_MASK);
	}
}


StringList
StringListOperations::Apply(const StringList& inputList, size_t maxSize,
	const behavior::Behavior& behavior) const
{
	if (!HasOperations())
		return inputList;

	uint32_t operations = fOperations;
	bool hasSelectorOperation = (operations & PATH_PART_SELECTOR_MASK) != 0;
	bool hasPathPartOperation = hasSelectorOperation
		|| (operations & PATH_PART_REPLACER_MASK) != 0;
	if (hasPathPartOperation && !hasSelectorOperation) {
		// Only replacer operations. Continue as if all path parts are selected.
		operations |= PATH_PART_SELECTOR_MASK;
	}

	StringList resultList;
	StringBuffer buffer;

	const StringList& list
		= inputList.IsEmpty() && (operations & REPLACE_EMPTY) != 0
			? StringList(String(fEmptyParameter)) : inputList;

	size_t count = std::min(list.Size(), maxSize);
	for (size_t i = 0; i < count; i++) {
		String string = list.ElementAt(i);

		size_t bufferSizeBeforeElement = buffer.Length();

		// If any of the file name/path part selectors/replacers need to be
		// applied, we disassemble the string, make the modifications, and
		// reassemble it.
		if (hasPathPartOperation) {
			PathParts parts;
			_DisassemblePath(string, parts);

			if ((operations & REPLACE_GRIST) != 0)
				parts.fGrist = fGristParameter;
			else if ((operations & SELECT_GRIST) == 0)
				parts.fGrist.Unset();

			if ((operations & REPLACE_ROOT) != 0)
				parts.fRoot = fRootParameter;
			else if ((operations & SELECT_ROOT) == 0)
				parts.fRoot.Unset();

			if ((operations & REPLACE_DIRECTORY) != 0)
				parts.fDirectory = fDirectoryParameter;
			else if ((operations & SELECT_DIRECTORY) == 0)
				parts.fDirectory.Unset();

			if ((operations & REPLACE_BASE_NAME) != 0)
				parts.fBaseName = fBaseNameParameter;
			else if ((operations & SELECT_BASE_NAME) == 0)
				parts.fBaseName.Unset();

			if ((operations & REPLACE_SUFFIX) != 0)
				parts.fSuffix = fSuffixParameter;
			else if ((operations & SELECT_SUFFIX) == 0)
				parts.fSuffix.Unset();

			if ((operations & REPLACE_ARCHIVE_MEMBER) != 0)
				parts.fArchiveMember = fArchiveMemberParameter;
			else if ((operations & SELECT_ARCHIVE_MEMBER) == 0)
				parts.fArchiveMember.Unset();

			if ((operations & TO_PARENT_DIRECTORY) != 0) {
				parts.fBaseName.Unset();
				parts.fSuffix.Unset();
				parts.fArchiveMember.Unset();
			}

			_AssemblePath(parts, buffer, behavior);
		} else
			buffer += string;

		if ((operations & TO_UPPER) != 0) {
			char* toConvert = buffer.Data() + bufferSizeBeforeElement;
			char* end = buffer.Data() + buffer.Length();
			std::transform(toConvert, end, toConvert, ::toupper);
		} else if ((operations & TO_LOWER) != 0) {
			char* toConvert = buffer.Data() + bufferSizeBeforeElement;
			char* end = buffer.Data() + buffer.Length();
			std::transform(toConvert, end, toConvert, ::tolower);
		}

		if ((operations & JOIN) != 0) {
			if (i + 1 < count)
				buffer += fJoinParameter;
		} else {
			resultList.Append(buffer);
			buffer = StringPart();
		}
	}

	if ((operations & JOIN) != 0 && count > 0)
		resultList.Append(buffer);

	return resultList;
}


/*static*/ void
StringListOperations::_DisassemblePath(const String& path, PathParts& _parts)
{
	// TODO: This is platform dependent!

	const char* remainder = path.ToCString();
	const char* pathEnd = remainder + path.Length();

	// grist
	if (*remainder == '<') {
		const char* gristStart = remainder;
		remainder = std::find(remainder + 1, pathEnd, '>');
		if (remainder != pathEnd) {
			remainder++;
			_parts.fGrist.SetTo(gristStart, remainder);
		} else {
			// no/broken grist
			_parts.fGrist.Unset();
			remainder = gristStart;
		}
	} else
		_parts.fGrist.Unset();

	// root
	_parts.fRoot.Unset();

	// directory path
	if (const char* lastSlash = strrchr(remainder, '/')) {
		_parts.fDirectory.SetTo(remainder,
			lastSlash == remainder ? remainder + 1 : lastSlash);
		remainder = lastSlash + 1;
	} else
		_parts.fDirectory.Unset();

	// archive member
	const char* archiveMemberStart = NULL;
	if (remainder != pathEnd && pathEnd[-1] == ')')
		archiveMemberStart = strchr(remainder, '(');
	if (archiveMemberStart != NULL)
		_parts.fArchiveMember.SetTo(archiveMemberStart + 1 , pathEnd - 1);
	else
		_parts.fArchiveMember.Unset();

	// suffix
	const char* fileNameEnd = archiveMemberStart != NULL
		? archiveMemberStart : pathEnd;
	typedef std::reverse_iterator<const char*> ReverseStringIterator;
	const char* lastDot = std::find(ReverseStringIterator(fileNameEnd),
		ReverseStringIterator(remainder), '.').base() - 1;
	if (lastDot != remainder - 1) {
		_parts.fSuffix.SetTo(lastDot, fileNameEnd);
		fileNameEnd = lastDot;
	} else
		_parts.fSuffix.Unset();

	// base name
	_parts.fBaseName.SetTo(remainder, fileNameEnd);
}


/*static*/ void
StringListOperations::_AssemblePath(const PathParts& parts,
	StringBuffer& buffer, const behavior::Behavior& behavior)
{
	// TODO: This is platform dependent!

	if (!parts.fGrist.IsEmpty()) {
		if (parts.fGrist.Start()[0] != '<')
			buffer += '<';
		buffer += parts.fGrist;
		if (parts.fGrist.End()[-1] != '>')
			buffer += '>';
	}

	// Use root only, if the directory part isn't absolute and if the root isn't
	// ".".
	if (!parts.fRoot.IsEmpty()
		&& (parts.fRoot.Length() > 1 || parts.fRoot.Start()[0] != '.')
		&& (parts.fDirectory.IsEmpty() || parts.fDirectory.Start()[0] != '/')) {
		buffer += parts.fRoot;
		if (behavior.GetPathRootReplacerSlash()
				== behavior::Behavior::PATH_ROOT_REPLACER_SLASH_ALWAYS
			|| parts.fRoot.End()[-1] != '/') {
			buffer += '/';
		}
	}

	if (!parts.fDirectory.IsEmpty()) {
		buffer += parts.fDirectory;
		if (parts.fDirectory != StringPart("/", 1)
			&& (!parts.fBaseName.IsEmpty() || !parts.fSuffix.IsEmpty())) {
			buffer += '/';
		}
	}

	buffer += parts.fBaseName;
	buffer += parts.fSuffix;

	if (!parts.fArchiveMember.IsEmpty()) {
		buffer += '(';
		buffer += parts.fArchiveMember;
		buffer += ')';
	}
}


}	// namespace data
}	// namespace ham
