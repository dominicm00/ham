/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "data/StringListOperations.h"

#include "data/Path.h"


namespace ham {
namespace data {


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
		// Emulate Jam behavior: On the first encounter of a path part selector
		// we forget all path part replacer operations.
		if ((pendingOperation & PATH_PART_SELECTOR_MASK) != 0
			&& (fOperations & PATH_PART_SELECTOR_MASK) == 0
			&& (fOperations & PATH_PART_REPLACER_MASK) != 0) {
			fGristParameter.Unset();
			fDirectoryParameter.Unset();
			fBaseNameParameter.Unset();
			fSuffixParameter.Unset();
			fArchiveMemberParameter.Unset();
			fRootParameter.Unset();
			fOperations &= ~(uint32_t)PATH_PART_REPLACER_MASK;
		}

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
		return inputList.SubList(0, maxSize);

	uint32_t operations = fOperations;
	bool hasSelectorOperation = (operations & PATH_PART_SELECTOR_MASK) != 0;
	bool hasPathPartOperation = hasSelectorOperation
		|| (operations & (PATH_PART_REPLACER_MASK | TO_PARENT_DIRECTORY)) != 0;
	if (hasPathPartOperation && !hasSelectorOperation) {
		// Only replacer operations. Continue as if all path parts are selected.
		operations |= PATH_PART_SELECTOR_MASK;
	}

	// If a join shall be performed before to-upper/to-lower, we simply convert
	// the join parameter first and join as usual afterwards.
	String joinParameterBuffer;
	StringPart joinParameter = fJoinParameter;
	if (!joinParameter.IsEmpty()
		&& (operations & (TO_UPPER | TO_LOWER)) != 0
		&& behavior.GetJoinCaseOperator()
			== behavior::Behavior::JOIN_BEFORE_CASE_OPERATOR) {
		joinParameterBuffer = joinParameter;
		if ((operations & TO_UPPER) != 0)
			joinParameterBuffer.ToUpper();
		else
			joinParameterBuffer.ToLower();
		joinParameter = joinParameterBuffer;
	}

	StringList resultList;
	StringBuffer buffer;

	const StringList& list
		= inputList.IsEmpty() && (operations & REPLACE_EMPTY) != 0
				&& !fEmptyParameter.IsEmpty()
			? StringList(String(fEmptyParameter)) : inputList;

	size_t count = std::min(list.Size(), maxSize);
	for (size_t i = 0; i < count; i++) {
		String string = list.ElementAt(i);

		size_t bufferSizeBeforeElement = buffer.Length();

		// If any of the file name/path part selectors/replacers need to be
		// applied, we disassemble the string, make the modifications, and
		// reassemble it.
		if (hasPathPartOperation) {
			Path::Parts parts(string);

			if ((operations & REPLACE_GRIST) != 0)
				parts.SetGrist(fGristParameter);
			else if ((operations & SELECT_GRIST) == 0)
				parts.UnsetGrist();

			if ((operations & REPLACE_ROOT) != 0)
				parts.SetRoot(fRootParameter);
			else if ((operations & SELECT_ROOT) == 0)
				parts.UnsetRoot();

			if ((operations & REPLACE_DIRECTORY) != 0)
				parts.SetDirectory(fDirectoryParameter);
			else if ((operations & SELECT_DIRECTORY) == 0)
				parts.UnsetDirectory();

			if ((operations & REPLACE_BASE_NAME) != 0)
				parts.SetBaseName(fBaseNameParameter);
			else if ((operations & SELECT_BASE_NAME) == 0)
				parts.UnsetBaseName();

			if ((operations & REPLACE_SUFFIX) != 0)
				parts.SetSuffix(fSuffixParameter);
			else if ((operations & SELECT_SUFFIX) == 0)
				parts.UnsetSuffix();

			if ((operations & REPLACE_ARCHIVE_MEMBER) != 0)
				parts.SetArchiveMember(fArchiveMemberParameter);
			else if ((operations & SELECT_ARCHIVE_MEMBER) == 0)
				parts.UnsetArchiveMember();

			if ((operations & TO_PARENT_DIRECTORY) != 0) {
				parts.UnsetBaseName();
				parts.UnsetSuffix();
				parts.UnsetArchiveMember();
			}

			parts.GetPath(buffer, behavior);
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
				buffer += joinParameter;
		} else {
			resultList.Append(buffer);
			buffer = StringPart();
		}
	}

	if ((operations & JOIN) != 0 && count > 0) {
		// Append the joined value, if we're not emulating the broken behavior
		// of jam.
		if (behavior.GetBrokenSubscriptJoin()
				== behavior::Behavior::NO_BROKEN_SUBSCRIPT_JOIN
			|| count == list.Size()) {
			resultList.Append(buffer);
		}
	}

	return resultList;
}


}	// namespace data
}	// namespace ham
