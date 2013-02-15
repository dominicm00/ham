/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_TARGET_H
#define HAM_DATA_TARGET_H


#include "data/VariableDomain.h"


namespace ham {
namespace data {


class Target {
public:
			enum {
				BUILD_ALWAYS		= 0x01,
					// rebuild regardless of date of pre-existing file
				DONT_UPDATE			= 0x02,
					// don't update, if file exists, regardless of file date
				IGNORE_IF_MISSING	= 0x04,
					// silently ignore the (source) target, if file is missing
				NOT_A_FILE			= 0x08,
					// pseudo target
				TEMPORARY			= 0x10,
				DEPENDS_ON_LEAVES	= 0x20
					// target depends on its leaf dependencies only,
					// intermediate dependencies are ignored
			};

public:
								Target();
								Target(const String& name);
								~Target();

			void				SetName(const String& name)
									{ fName = name; }
									// conceptually package private

	inline	VariableDomain*		Variables(bool create);

			uint32_t			Flags() const
									{ return fFlags; }
			void				SetFlags(uint32_t flags)
									{ fFlags = flags; }
			void				AddFlags(uint32_t flags)
									{ fFlags |= flags; }

private:
			String				fName;
			VariableDomain*		fVariables;
			uint32_t			fFlags;
};


VariableDomain*
Target::Variables(bool create)
{
	if (fVariables == NULL && create)
		fVariables = new VariableDomain;

	return fVariables;
}


} // namespace data
} // namespace ham


#endif	// HAM_DATA_TARGET_H
