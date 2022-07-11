/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_STRING_LIST_OPERATIONS_HPP
#define HAM_DATA_STRING_LIST_OPERATIONS_HPP

#include "behavior/Behavior.hpp"
#include "data/Path.hpp"

#include <concepts>
#include <iterator>
#include <ranges>
#include <stdint.h>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace ham::data
{

template<typename List>
concept StringRange = std::ranges::forward_range<
	List> && std::same_as<std::ranges::range_value_t<List>, std::string_view>;

class StringListOperations
{
  public:
	enum {
		REPLACE_GRIST = 0x00001,
		REPLACE_ROOT = 0x00002,
		REPLACE_DIRECTORY = 0x00004,
		REPLACE_BASE_NAME = 0x00008,
		REPLACE_SUFFIX = 0x00010,
		REPLACE_ARCHIVE_MEMBER = 0x00020,
		TO_PARENT_DIRECTORY = 0x00040,
		REPLACE_EMPTY = 0x00080,
		JOIN = 0x00100,

		SELECT_GRIST = 0x00200,
		SELECT_ROOT = 0x00400,
		SELECT_DIRECTORY = 0x00800,
		SELECT_BASE_NAME = 0x01000,
		SELECT_SUFFIX = 0x02000,
		SELECT_ARCHIVE_MEMBER = 0x04000,

		TO_LOWER = 0x08000,
		TO_UPPER = 0x10000,

		PATH_PART_REPLACER_MASK = REPLACE_GRIST | REPLACE_ROOT
			| REPLACE_DIRECTORY | REPLACE_BASE_NAME | REPLACE_SUFFIX
			| REPLACE_ARCHIVE_MEMBER,
		PATH_PART_SELECTOR_MASK = SELECT_GRIST | SELECT_ROOT | SELECT_DIRECTORY
			| SELECT_BASE_NAME | SELECT_SUFFIX | SELECT_ARCHIVE_MEMBER,
		PARAMETER_OPERATION_MASK = REPLACE_GRIST | REPLACE_ROOT
			| REPLACE_DIRECTORY | REPLACE_BASE_NAME | REPLACE_SUFFIX
			| REPLACE_ARCHIVE_MEMBER | REPLACE_EMPTY | JOIN,
		NO_PARAMETER_OPERATION_MASK = TO_PARENT_DIRECTORY | SELECT_GRIST
			| SELECT_ROOT | SELECT_DIRECTORY | SELECT_BASE_NAME | SELECT_SUFFIX
			| SELECT_ARCHIVE_MEMBER | TO_LOWER | TO_UPPER
	};

  public:
	StringListOperations();

	void Parse(std::string_view);

	inline uint32_t Operations() const { return fOperations; }
	inline bool HasOperations() const { return fOperations != 0; }

	inline void AddOperations(uint32_t operations)
	{
		fOperations |= operations;
	}

	std::vector<std::string> Apply(
		StringRange auto inputList,
		size_t maxSize,
		const behavior::Behavior& behavior
	) const;

  private:
	uint32_t fOperations;
	std::string_view fGristParameter;
	std::string_view fDirectoryParameter;
	std::string_view fBaseNameParameter;
	std::string_view fSuffixParameter;
	std::string_view fArchiveMemberParameter;
	std::string_view fRootParameter;
	std::string_view fEmptyParameter;
	std::string_view fJoinParameter;
};

} // namespace ham::data

#endif // HAM_DATA_STRING_LIST_OPERATIONS_HPP
