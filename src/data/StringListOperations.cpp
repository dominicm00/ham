/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "data/StringListOperations.hpp"

#include "data/Path.hpp"
#include "util/StringUtil.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace ham::data
{

// #pragma mark - StringListOperations

StringListOperations::StringListOperations()
	: fOperations(0),
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
StringListOperations::Parse(std::string_view str)
{
	uint32_t pendingOperation = 0;
	std::string_view* pendingParameter = nullptr;

	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] == '=') {
			if (pendingParameter != nullptr) {
				*pendingParameter = str.substr(i + 1);
				AddOperations(pendingOperation & PARAMETER_OPERATION_MASK);
			}
			return;
		}

		AddOperations(pendingOperation & NO_PARAMETER_OPERATION_MASK);
		pendingOperation = 0;
		pendingParameter = nullptr;

		switch (str[i]) {
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
				pendingOperation =
					SELECT_ARCHIVE_MEMBER | REPLACE_ARCHIVE_MEMBER;
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
		// TODO: Make this compatibility?
		if ((pendingOperation & PATH_PART_SELECTOR_MASK) != 0
			&& (fOperations & PATH_PART_SELECTOR_MASK) == 0
			&& (fOperations & PATH_PART_REPLACER_MASK) != 0) {
			fGristParameter = {};
			fDirectoryParameter = {};
			fBaseNameParameter = {};
			fSuffixParameter = {};
			fArchiveMemberParameter = {};
			fRootParameter = {};
			fOperations &= ~(uint32_t)PATH_PART_REPLACER_MASK;
		}

		AddOperations(pendingOperation & NO_PARAMETER_OPERATION_MASK);
	} else if (pendingParameter != nullptr) {
		*pendingParameter = {};
		AddOperations(pendingOperation & PARAMETER_OPERATION_MASK);
	}
}

std::vector<std::string>
StringListOperations::Apply(
	StringRange auto inputList,
	size_t maxSize,
	const behavior::Behavior& behavior
) const
{
	if (!HasOperations())
		return inputList | std::ranges::views::take(maxSize);

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
	std::string joinParameter{fJoinParameter};
	if (!joinParameter.empty() && (operations & (TO_UPPER | TO_LOWER)) != 0
		&& behavior.GetJoinCaseOperator()
			== behavior::Behavior::JOIN_BEFORE_CASE_OPERATOR) {
		if ((operations & TO_UPPER) != 0)
			joinParameter = util::String::ToUpper(fJoinParameter);
		else
			joinParameter = util::String::ToLower(fJoinParameter);
	}

	std::vector<std::string> resultList;
	std::string buffer;

	const std::vector<std::string> list =
		inputList.empty() && (operations & REPLACE_EMPTY) != 0
		? std::vector<std::string>{std::string{fEmptyParameter}}
		: inputList;

	size_t count = std::min(list.size(), maxSize);
	for (size_t i = 0; i < count; i++) {
		std::string str = list[i];

		size_t bufferSizeBeforeElement = buffer.length();

		// If any of the file name/path part selectors/replacers need to be
		// applied, we disassemble the string, make the modifications, and
		// reassemble it.
		if (hasPathPartOperation) {
			Path::Parts parts{str};

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

			buffer = parts.ToPath(behavior);
		} else {
			buffer += str;
		}

		if ((operations & TO_UPPER) != 0) {
			std::transform(
				buffer.cbegin() + bufferSizeBeforeElement,
				buffer.cend(),
				buffer.begin() + bufferSizeBeforeElement,
				[](unsigned char c) { return std::toupper(c); }
			);
		} else if ((operations & TO_LOWER) != 0) {
			std::transform(
				buffer.cbegin() + bufferSizeBeforeElement,
				buffer.cend(),
				buffer.begin() + bufferSizeBeforeElement,
				[](unsigned char c) { return std::tolower(c); }
			);
		}

		if ((operations & JOIN) != 0) {
			if (i + 1 < count)
				buffer += joinParameter;
		} else {
			resultList.push_back(buffer);
			buffer = std::string{};
		}
	}

	if ((operations & JOIN) != 0 && count > 0) {
		// Append the joined value, if we're not emulating the broken behavior
		// of jam.
		if (behavior.GetBrokenSubscriptJoin()
				== behavior::Behavior::NO_BROKEN_SUBSCRIPT_JOIN
			|| count == list.size()) {
			resultList.push_back(buffer);
		}
	}

	return resultList;
}

} // namespace ham::data
