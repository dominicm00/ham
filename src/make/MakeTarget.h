/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_MAKE_MAKE_TARGET_H
#define HAM_MAKE_MAKE_TARGET_H


#include "data/FileStatus.h"
#include "data/Target.h"


namespace ham {
namespace make {


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

			enum MakeState {
				DONE,
				PENDING,
				FAILED,
				SKIPPED
			};

public:
								MakeTarget(data::Target* target);
								~MakeTarget();

			String				Name() const
									{ return fTarget->Name(); }

			data::Target*		GetTarget() const
									{ return fTarget; }

			bool				IsBound() const
									{ return !fBoundPath.IsEmpty(); }
			String				BoundPath() const
									{ return fBoundPath; }
			void				SetBoundPath(const String& path)
									{ fBoundPath = path; }

			data::Time			GetTime() const
									{ return fTime; }
			void				SetTime(const data::Time& time)
									{ fTime = time; }

			data::Time			LeafTime() const
									{ return fLeafTime; }
			void				SetLeafTime(const data::Time& time)
									{ fLeafTime = time; }

			bool				FileExists() const
									{ return fFileExists; }
			void				SetFileStatus(
									const data::FileStatus& fileStatus);

			const MakeTargetSet& Dependencies() const
									{ return fDependencies; }
			void				AddDependency(MakeTarget* dependency)
									{ fDependencies.Append(dependency); }
	inline	void				AddDependencies(
									const MakeTargetSet& dependencies);
			bool				IsLeaf() const
									{ return fDependencies.IsEmpty(); }

			const MakeTargetSet& Includes() const
									{ return fIncludes; }
			void				AddInclude(MakeTarget* include)
									{ fIncludes.Append(include); }
	inline	void				AddIncludes(const MakeTargetSet& includes);

			const MakeTargetSet& Parents() const
									{ return fParents; }
			void				AddParent(MakeTarget* parent)
									{ fParents.Append(parent); }

			State				GetState() const
									{ return fState; }
			void				SetState(State state)
									{ fState = state; }

			Fate				GetFate() const
									{ return fFate; }
			void				SetFate(Fate fate)
									{ fFate = fate; }

			MakeState			GetMakeState() const
									{ return fMakeState; }
			void				SetMakeState(MakeState state)
									{ fMakeState = state; }

			size_t				PendingDependenciesCount() const
									{ return fPendingDependencyCount; }
			void				SetPendingDependenciesCount(size_t count)
									{ fPendingDependencyCount = count; }

private:
			data::Target*		fTarget;
			String				fBoundPath;
			data::Time			fTime;
			data::Time			fLeafTime;
			bool				fFileExists;
			MakeTargetSet		fDependencies;
			MakeTargetSet		fIncludes;
			MakeTargetSet		fParents;
			State				fState;
			Fate				fFate;
			MakeState			fMakeState;
			size_t				fPendingDependencyCount;
};


void
MakeTarget::AddDependencies(const MakeTargetSet& dependencies)
{
	for (MakeTargetSet::Iterator it = dependencies.GetIterator(); it.HasNext();)
		AddDependency(it.Next());
}


void
MakeTarget::AddIncludes(const MakeTargetSet& includes)
{
	for (MakeTargetSet::Iterator it = includes.GetIterator(); it.HasNext();)
		AddInclude(it.Next());
}


} // namespace make
} // namespace ham


#endif	// HAM_MAKE_MAKE_TARGET_H
