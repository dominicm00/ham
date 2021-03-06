/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Copyright 2022, Dominic Martinez, dom@dominicm.dev.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_MAKE_MAKE_TARGET_HPP
#define HAM_MAKE_MAKE_TARGET_HPP

#include "data/FileStatus.hpp"
#include "data/Target.hpp"

namespace ham::make
{

class MakeTarget;

typedef util::SequentialSet<MakeTarget*> MakeTargetSet;

class MakeTarget
{
  public:
	/**
	 * State during recursive processing. Used while processing the make tree to
	 * detect cycles.
	 */
	enum ProcessingState {
		UNPROCESSED,
		PROCESSING,
		PROCESSED
	};

	/**
	 * File status of the target.
	 */
	enum State {
		UP_TO_DATE,
		OUT_OF_DATE,
		MISSING
	};

	/**
	 * Whether or not to make a target.
	 */
	enum Fate {
		MAKE,
		MAKE_IF_NEEDED,
		KEEP,
		CANT_MAKE
	};

	/**
	 * Status of make execution.
	 */
	enum MakeState {
		DONE,
		PENDING,
		FAILED,
		SKIPPED
	};

  public:
	MakeTarget(data::Target* target);
	~MakeTarget();

	String Name() const { return fTarget->Name(); }

	data::Target* GetTarget() const { return fTarget; }

	bool IsBound() const { return !fBoundPath.IsEmpty(); }
	String BoundPath() const { return fBoundPath; }
	void SetBoundPath(const String& path) { fBoundPath = path; }

	data::Time GetOriginalTime() const { return fOriginalTime; }
	void SetOriginalTime(const data::Time& time) { fOriginalTime = time; }

	data::Time GetTime() const { return fTime; }
	void SetTime(const data::Time& time) { fTime = time; }

	data::Time LeafTime() const { return fLeafTime; }
	void SetLeafTime(const data::Time& time) { fLeafTime = time; }

	bool FileExists() const { return fFileExists; }
	void SetFileStatus(const data::FileStatus& fileStatus);

	const MakeTargetSet& Dependencies() const { return fDependencies; }
	void AddDependency(MakeTarget* dependency)
	{
		fDependencies.Append(dependency);
	}
	inline void AddDependencies(const MakeTargetSet& dependencies);
	bool IsLeaf() const { return fDependencies.IsEmpty(); }

	const MakeTargetSet& Includes() const { return fIncludes; }
	void AddInclude(MakeTarget* include) { fIncludes.Append(include); }
	inline void AddIncludes(const MakeTargetSet& includes);

	const MakeTargetSet& Parents() const { return fParents; }
	void AddParent(MakeTarget* parent) { fParents.Append(parent); }

	ProcessingState GetProcessingState() const { return fProcessingState; }
	void SetProcessingState(ProcessingState state) { fProcessingState = state; }

	State GetState() const { return fState; }
	void SetState(State state) { fState = state; }

	Fate GetFate() const { return fFate; }
	void SetFate(Fate fate) { fFate = fate; }

	MakeState GetMakeState() const { return fMakeState; }
	void SetMakeState(MakeState state) { fMakeState = state; }

	size_t PendingDependenciesCount() const { return fPendingDependencyCount; }
	void SetPendingDependenciesCount(size_t count)
	{
		fPendingDependencyCount = count;
	}

  private:
	data::Target* fTarget;
	String fBoundPath;
	data::Time fOriginalTime;
	// file time, or, if missing, least possible
	data::Time fTime;
	// "make" time -- may be file time or
	// time inherited from dependencies
	data::Time fLeafTime;
	// == fTime, if leaf, otherwise the time of
	// the newest leaf dependency
	bool fFileExists;
	MakeTargetSet fDependencies;
	MakeTargetSet fIncludes;
	MakeTargetSet fParents;
	ProcessingState fProcessingState;
	State fState;
	Fate fFate;
	MakeState fMakeState;
	size_t fPendingDependencyCount;
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

} // namespace ham::make

#endif // HAM_MAKE_MAKE_TARGET_HPP
