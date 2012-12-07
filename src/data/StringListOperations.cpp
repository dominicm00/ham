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
				break;
			case 'P':
				pendingOperation = SELECT_PARENT_DIRECTORY;
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
StringListOperations::Apply(const StringList& inputList, size_t maxSize) const
{
	if (!HasOperations())
		return inputList;

	bool hasPathPartOperation = (fOperations & PATH_PART_SELECTOR_MASK) != 0
		|| (fOperations & PATH_PART_REPLACER_MASK) != 0;

	StringList resultList;
	StringBuffer buffer;

	const StringList& list
		= inputList.IsEmpty() && (fOperations & REPLACE_EMPTY) != 0
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

			if ((fOperations & REPLACE_GRIST) != 0)
				parts.fGrist = fGristParameter;
			else if ((fOperations & SELECT_GRIST) == 0)
				parts.fGrist.Unset();

			if ((fOperations & REPLACE_ROOT) != 0)
				parts.fRoot = fRootParameter;
			else if ((fOperations & SELECT_ROOT) == 0)
				parts.fRoot.Unset();

			// TODO: Correct handling of SELECT_PARENT_DIRECTORY!
			if ((fOperations & REPLACE_DIRECTORY) != 0)
				parts.fDirectory = fDirectoryParameter;
			else if ((fOperations
					& (SELECT_DIRECTORY | SELECT_PARENT_DIRECTORY)) == 0) {
				parts.fDirectory.Unset();
			}

			if ((fOperations & REPLACE_BASE_NAME) != 0)
				parts.fBaseName = fBaseNameParameter;
			else if ((fOperations & SELECT_BASE_NAME) == 0)
				parts.fBaseName.Unset();

			if ((fOperations & REPLACE_SUFFIX) != 0)
				parts.fSuffix = fSuffixParameter;
			else if ((fOperations & SELECT_SUFFIX) == 0)
				parts.fSuffix.Unset();

			if ((fOperations & REPLACE_ARCHIVE_MEMBER) != 0)
				parts.fArchiveMember = fArchiveMemberParameter;
			else if ((fOperations & SELECT_ARCHIVE_MEMBER) == 0)
				parts.fArchiveMember.Unset();

			_AssemblePath(parts, buffer);
		} else
			buffer += string;

		if ((fOperations & TO_UPPER) != 0) {
			char* toConvert = buffer.Data() + bufferSizeBeforeElement;
			char* end = buffer.Data() + buffer.Length();
			std::transform(toConvert, end, toConvert, ::toupper);
		} else if ((fOperations & TO_LOWER) != 0) {
			char* toConvert = buffer.Data() + bufferSizeBeforeElement;
			char* end = buffer.Data() + buffer.Length();
			std::transform(toConvert, end, toConvert, ::tolower);
		}

		if ((fOperations & JOIN) != 0) {
			if (i + 1 < count)
				buffer += fJoinParameter;
		} else {
			resultList.Append(buffer);
			buffer = StringPart();
		}
	}

	if ((fOperations & JOIN) != 0 && count > 0)
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
		for (remainder++; *remainder != '\0'; remainder++) {
			if (*remainder == '>') {
				remainder++;
				break;
			}
		}

		_parts.fGrist.SetTo(gristStart, remainder);
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
		_parts.fArchiveMember.SetTo(archiveMemberStart, pathEnd);
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
	StringBuffer& buffer)
{
	// TODO: This is platform dependent!

	buffer += parts.fGrist;
	buffer += parts.fRoot;
	buffer += parts.fDirectory;
	if (!parts.fDirectory.IsEmpty() && parts.fDirectory != StringPart("/", 1)
		&& (!parts.fBaseName.IsEmpty() || !parts.fSuffix.IsEmpty()
			|| !parts.fArchiveMember.IsEmpty())) {
		buffer += '/';
	}

	buffer += parts.fBaseName;
	buffer += parts.fSuffix;
	buffer += parts.fArchiveMember;
}


}	// namespace data
}	// namespace ham
