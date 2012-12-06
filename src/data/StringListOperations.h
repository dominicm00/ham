/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_STRING_LIST_OPERATIONS_H
#define HAM_DATA_STRING_LIST_OPERATIONS_H


#include "data/StringBuffer.h"
#include "data/StringList.h"
#include "data/StringPart.h"


namespace ham {
namespace data {


class StringListOperations {
public:
			enum {
				REPLACE_GRIST				= 0x00001,
				REPLACE_ROOT				= 0x00002,
				REPLACE_DIRECTORY			= 0x00004,
				REPLACE_BASE_NAME			= 0x00008,
				REPLACE_SUFFIX				= 0x00010,
				REPLACE_ARCHIVE_MEMBER		= 0x00020,
				REPLACE_EMPTY				= 0x00040,
				JOIN						= 0x00080,

				SELECT_GRIST				= 0x00100,
				SELECT_ROOT					= 0x00200,
				SELECT_PARENT_DIRECTORY		= 0x00400,
				SELECT_DIRECTORY			= 0x00800,
				SELECT_BASE_NAME			= 0x01000,
				SELECT_SUFFIX				= 0x02000,
				SELECT_ARCHIVE_MEMBER		= 0x04000,

				TO_LOWER					= 0x08000,
				TO_UPPER					= 0x10000,

				PATH_PART_REPLACER_MASK		= 0x0002f,
				PATH_PART_SELECTOR_MASK		= 0x07e00,
				PARAMETER_OPERATION_MASK	= 0x001ff,
				NO_PARAMETER_OPERATION_MASK	= 0x1fe00
			};

public:
								StringListOperations();

			void				Parse(const char* start,
									const char* end);

	inline	uint32_t			Operations() const
									{ return fOperations; }
	inline	bool				HasOperations() const
									{ return fOperations != 0; }

	inline	void				AddOperations(uint32_t operations)
									{ fOperations |= operations; }

			StringList			Apply(const StringList& list) const;

private:
			struct PathParts;

private:
	static	void				_DisassemblePath(const String& path,
									PathParts& _parts);
	static	void				_AssemblePath(const PathParts& parts,
									StringBuffer& buffer);

private:
			uint32_t			fOperations;
			StringPart			fGristParameter;
			StringPart			fDirectoryParameter;
			StringPart			fBaseNameParameter;
			StringPart			fSuffixParameter;
			StringPart			fArchiveMemberParameter;
			StringPart			fRootParameter;
			StringPart			fEmptyParameter;
			StringPart			fJoinParameter;
};


}	// namespace data
}	// namespace ham


#endif // HAM_DATA_STRING_LIST_OPERATIONS_H
