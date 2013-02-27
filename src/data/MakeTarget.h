/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_MAKE_TARGET_H
#define HAM_DATA_MAKE_TARGET_H


#include "data/FileStatus.h"
#include "data/Target.h"


namespace ham {
namespace data {


class MakeTarget;

typedef util::SequentialSet<MakeTarget*> MakeTargetSet;


class MakeTarget {
public:
			enum State {
				UP_TO_DATE,
				OUT_OF_DATE,
				MISSING
			};

			enum Fate {
				UNPROCESSED,
				PROCESSING,
				MAKE,
				KEEP,
				CANT_MAKE
			};

public:
								MakeTarget(Target* target);
								~MakeTarget();

			String				Name() const
									{ return fTarget->Name(); }

			Target*				GetTarget() const
									{ return fTarget; }

			bool				IsBound() const
									{ return !fBoundPath.IsEmpty(); }
			String				BoundPath() const
									{ return fBoundPath; }
			void				SetBoundPath(const String& path)
									{ fBoundPath = path; }

			Time				GetTime() const
									{ return fTime; }
			void				SetTime(const Time& time)
									{ fTime = time; }

			Time				LeafTime() const
									{ return fLeafTime; }
			void				SetLeafTime(const Time& time)
									{ fLeafTime = time; }

			bool				FileExists() const
									{ return fFileExists; }
			void				SetFileStatus(const FileStatus& fileStatus);

			const MakeTargetSet& Dependencies() const
									{ return fDependencies; }
			void				AddDependency(MakeTarget* dependency)
									{ fDependencies.insert(dependency); }
	inline	void				AddDependencies(
									const MakeTargetSet& dependencies);
			bool				IsLeaf() const
									{ return fDependencies.size() == 0; }

			const MakeTargetSet& Includes() const
									{ return fIncludes; }
			void				AddInclude(MakeTarget* include)
									{ fIncludes.insert(include); }
	inline	void				AddIncludes(const MakeTargetSet& includes);

			State				GetState() const
									{ return fState; }
			void				SetState(State state)
									{ fState = state; }

			Fate				GetFate() const
									{ return fFate; }
			void				SetFate(Fate fate)
									{ fFate = fate; }

private:
			Target*				fTarget;
			String				fBoundPath;
			Time				fTime;
			Time				fLeafTime;
			bool				fFileExists;
			MakeTargetSet		fDependencies;
			MakeTargetSet		fIncludes;
			State				fState;
			Fate				fFate;
};


void
MakeTarget::AddDependencies(const MakeTargetSet& dependencies)
{
	for (MakeTargetSet::const_iterator it = dependencies.begin();
		it != dependencies.end(); ++it) {
		AddDependency(*it);
	}
}


void
MakeTarget::AddIncludes(const MakeTargetSet& includes)
{
	for (MakeTargetSet::const_iterator it = includes.begin();
		it != includes.end(); ++it) {
		AddInclude(*it);
	}
}


} // namespace data
} // namespace ham


#endif	// HAM_DATA_MAKE_TARGET_H
