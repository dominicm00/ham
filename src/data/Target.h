/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_TARGET_H
#define HAM_DATA_TARGET_H


#include "data/VariableDomain.h"
#include "util/SequentialSet.h"


namespace ham {
namespace data {


class Target;


typedef util::SequentialSet<Target*> TargetSet;


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

			const String&		Name() const
									{ return fName; }
			void				SetName(const String& name)
									{ fName = name; }
									// conceptually package private

	inline	VariableDomain*		Variables(bool create);
	inline	const VariableDomain* Variables() const
									{ return fVariables; }

			uint32_t			Flags() const
									{ return fFlags; }
			void				SetFlags(uint32_t flags)
									{ fFlags = flags; }
			void				AddFlags(uint32_t flags)
									{ fFlags |= flags; }
			bool				IsBuildAlways() const
									{ return (fFlags & BUILD_ALWAYS) != 0; }
			bool				IsDontUpdate() const
									{ return (fFlags & DONT_UPDATE) != 0; }
	inline	bool				IsIgnoreIfMissing() const;
			bool				IsNotAFile() const
									{ return (fFlags & NOT_A_FILE) != 0; }
			bool				IsTemporary() const
									{ return (fFlags & TEMPORARY) != 0; }
	inline	bool				DependsOnLeaves() const;

			const TargetSet&	Dependencies() const
									{ return fDependencies; }
			void				AddDependency(Target* dependency)
									{ fDependencies.insert(dependency); }
	inline	void				AddDependencies(const TargetSet& dependencies);

			const TargetSet&	Includes() const
									{ return fIncludes; }
			void				AddInclude(Target* include)
									{ fIncludes.insert(include); }
	inline	void				AddIncludes(const TargetSet& includes);

private:
			String				fName;
			VariableDomain*		fVariables;
			uint32_t			fFlags;
			TargetSet			fDependencies;
			TargetSet			fIncludes;
};


VariableDomain*
Target::Variables(bool create)
{
	if (fVariables == NULL && create)
		fVariables = new VariableDomain;

	return fVariables;
}


bool
Target::IsIgnoreIfMissing() const
{
	return (fFlags & IGNORE_IF_MISSING) != 0;
}


bool
Target::DependsOnLeaves() const
{
	return (fFlags & DEPENDS_ON_LEAVES) != 0;
}


void
Target::AddDependencies(const TargetSet& dependencies)
{
	for (TargetSet::const_iterator it = dependencies.begin();
		it != dependencies.end(); ++it) {
		AddDependency(*it);
	}
}


void
Target::AddIncludes(const TargetSet& includes)
{
	for (TargetSet::const_iterator it = includes.begin();
		it != includes.end(); ++it) {
		AddInclude(*it);
	}
}


} // namespace data
} // namespace ham


#endif	// HAM_DATA_TARGET_H
