/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_STRING_LIST_OPERATIONS_H
#define HAM_DATA_STRING_LIST_OPERATIONS_H


#include "data/StringList.h"


namespace ham {
namespace data {


class StringListOperations {
public:
			enum {
				REPLACE_GRIST				= 0x0001,
				REPLACE_DIRECTORY			= 0x0002,
				REPLACE_BASE_NAME			= 0x0003,
				REPLACE_SUFFIX				= 0x0004,
				REPLACE_ARCHIVE_MEMBER		= 0x0005,
				REPLACE_ROOT				= 0x0006,
				REPLACE_EMPTY				= 0x0007,
				JOIN						= 0x0008,

				SELECT_BASE_NAME			= 0x0010,
				SELECT_SUFFIX				= 0x0020,
				SELECT_ARCHIVE_MEMBER		= 0x0040,
				SELECT_DIRECTORY			= 0x0080,
				SELECT_PARENT_DIRECTORY		= 0x0100,
				SELECT_GRIST				= 0x0200,
				TO_LOWER					= 0x0400,
				TO_UPPER					= 0x0800,
				JOIN_NO_PARAMETER			= 0x1000,

				PATH_PART_REPLACER_MIN		= REPLACE_GRIST,
				PATH_PART_REPLACER_MAX		= REPLACE_ROOT,
				PATH_PART_SELECTOR_MASK		= 0x03f0,
				PARAMETER_OPERATION_MASK	= 0x000f,
				FLAGS_MASK					= 0xfff0
			};

public:
	inline						StringListOperations();

			bool				Parse(const char* start,
									const char* end);
	inline	void				Clear();

	inline	uint32_t			Operations() const
									{ return fOperations; }
	inline	bool				HasOperations() const
									{ return fOperations != 0; }

	inline	void				AddOperations(uint32_t operations)
									{ fOperations |= operations; }
	inline	void				SetParameterOperation(uint32_t operation,
									const char* parameterStart,
									const char* parameterEnd);

			StringList			Apply(const StringList& list) const;

private:
			struct PathParts;

private:
	static	void				_DisassemblePath(const String& path,
									PathParts& _parts);
	static	String				_AssemblePath(const PathParts& parts);

private:
			uint32_t			fOperations;
			const char*			fParameterStart;
			const char*			fParameterEnd;
};


inline
StringListOperations::StringListOperations()
{
	Clear();
}


inline void
StringListOperations::Clear()
{
	fOperations = 0;
	fParameterStart = NULL;
	fParameterEnd = NULL;
}


inline void
StringListOperations::SetParameterOperation(uint32_t operation,
	const char* parameterStart, const char* parameterEnd)
{
	fOperations |= operation;
	fParameterStart = parameterStart;
	fParameterEnd = parameterEnd;
}


}	// namespace data
}	// namespace ham


#endif // HAM_DATA_STRING_LIST_OPERATIONS_H
