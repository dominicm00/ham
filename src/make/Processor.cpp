/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "make/Processor.h"

#include <fstream>
#include <memory>

#include "code/Block.h"
#include "code/BuiltInRules.h"
#include "code/Jambase.h"
#include "data/TargetBinder.h"
#include "make/MakeException.h"
#include "parser/Parser.h"


namespace ham {
namespace make {


using data::Time;


Processor::Processor()
	:
	fGlobalVariables(),
	fTargets(),
	fEvaluationContext(fGlobalVariables, fTargets),
	fJambaseFile(),
	fActionsOutputFile(),
	fJobCount(1),
	fBuildFromNewest(false),
	fDryRun(false),
	fQuitOnError(false),
	fPrimaryTargetNames(),
	fMakeTargets()
{
	code::BuiltInRules::RegisterRules(fEvaluationContext.Rules());
}


Processor::~Processor()
{
	for (MakeTargetMap::iterator it = fMakeTargets.begin();
		it != fMakeTargets.end(); ++it) {
		delete it->second;
	}
}


void
Processor::SetCompatibility(behavior::Compatibility compatibility)
{
	fEvaluationContext.SetCompatibility(compatibility);
}


void
Processor::SetBehavior(behavior::Behavior behavior)
{
	fEvaluationContext.SetBehavior(behavior);
}


void
Processor::SetJambaseFile(const char* fileName)
{
	fJambaseFile = fileName;
}


void
Processor::SetActionsOutputFile(const char* fileName)
{
	fActionsOutputFile = fileName;
}


void
Processor::SetJobCount(int count)
{
	fJobCount = count;
}


void
Processor::SetBuildFromNewest(bool buildFromNewest)
{
	fBuildFromNewest = buildFromNewest;
}


void
Processor::SetDryRun(bool dryRun)
{
	fDryRun = dryRun;
}


void
Processor::SetQuitOnError(bool quitOnError)
{
	fQuitOnError = quitOnError;
}


void
Processor::SetOutput(std::ostream& output)
{
	fEvaluationContext.SetOutput(output);
}


void
Processor::SetErrorOutput(std::ostream& output)
{
	fEvaluationContext.SetErrorOutput(output);
}


void
Processor::SetPrimaryTargets(const StringList& targets)
{
	fPrimaryTargetNames = targets;
}


void
Processor::SetForceUpdateTargets(const StringList& targets)
{
	// TODO:...
}


void
Processor::ProcessJambase()
{
	// parse code
	parser::Parser parser;

	util::Reference<code::Block> block;

	if (fJambaseFile.IsEmpty()) {
		parser.SetFileName("Jambase");
		block.SetTo(parser.Parse(code::kJambase), true);
	} else {
		parser.SetFileName(fJambaseFile.ToStlString());
		block.SetTo(parser.ParseFile(fJambaseFile.ToCString()), true);
	}

	// execute the code
	block->Evaluate(fEvaluationContext);
}


void
Processor::PrepareTargets()
{
	fNow = Time::Now();

	// Create make targets for the given primary target names.
	if (fPrimaryTargetNames.IsEmpty())
		throw MakeException("No targets specified");

	size_t primaryTargetCount = fPrimaryTargetNames.Size();
	for (size_t i = 0; i < primaryTargetCount; i++) {
		String targetName = fPrimaryTargetNames.ElementAt(i);
		Target* target = fTargets.Lookup(targetName);
		if (target == NULL) {
			throw MakeException(std::string("Unknown target \"")
				+ targetName.ToCString() + "\"");
		}

		_GetMakeTarget(target, true);
	}

	// Bind the targets and their dependencies recursively.
	for (size_t i = 0; i < primaryTargetCount; i++) {
		String targetName = fPrimaryTargetNames.ElementAt(i);
		Target* target = fTargets.Lookup(targetName);
		_PrepareTargetRecursively(_GetMakeTarget(target, false));
	}
}


void
Processor::BuildTargets()
{
	// TODO:...
}


MakeTarget*
Processor::_GetMakeTarget(Target* target, bool create)
{
	MakeTargetMap::iterator it = fMakeTargets.find(target);
	if (it != fMakeTargets.end())
		return it->second;

	if (!create)
		return NULL;

	std::auto_ptr<MakeTarget> makeTarget(new MakeTarget(target));
	fMakeTargets[target] = makeTarget.get();
	return makeTarget.release();
}


void
Processor::_PrepareTargetRecursively(MakeTarget* makeTarget)
{
	// Check whether the target has already been processed (also detect cycles)
	// and mark in-progress.
	if (makeTarget->GetState() != MakeTarget::UNPROCESSED) {
		if (makeTarget->GetState() == MakeTarget::PROCESSING) {
			throw MakeException(std::string("Target \"")
				+ makeTarget->Name().ToCString() + "\" depends on itself");
		}

		// already done
		return;
	}

	makeTarget->SetState(MakeTarget::PROCESSING);

	// bind the target
	Target* target = makeTarget->GetTarget();
	bool isPseudoTarget = target->IsNotAFile();
	if (isPseudoTarget)
		makeTarget->SetTime(Time(0));
	else
		data::TargetBinder::Bind(fGlobalVariables, makeTarget);

	// header scanning
// TODO:...

	// add make targets for dependencies
	const TargetSet& dependencies = makeTarget->GetTarget()->Dependencies();
	for (TargetSet::iterator it = dependencies.begin();
		it != dependencies.end(); ++it) {
		makeTarget->AddDependency(_GetMakeTarget(*it, true));
	}

	// add make targets for includes
	const TargetSet& includes = makeTarget->GetTarget()->Includes();
	for (TargetSet::iterator it = includes.begin();
		it != includes.end(); ++it) {
		makeTarget->AddDependency(_GetMakeTarget(*it, true));
	}

	// recursively process the target's dependencies
	Time newestDependencyTime(0);
	Time newestLeafTime(0);
	bool dependencyUpdated = false;
	bool cantBuild = false;
	for (size_t i = 0; i < makeTarget->Dependencies().size(); i++) {
		MakeTarget* dependency = makeTarget->Dependencies().at(i);
		_PrepareTargetRecursively(dependency);

		// add the dependency's includes as the target's dependencies
		makeTarget->AddDependencies(dependency->Includes());

		// track times
		newestDependencyTime = std::max(newestDependencyTime,
			dependency->GetTime());
		newestLeafTime = std::max(newestLeafTime, dependency->LeafTime());

		switch (dependency->GetState()) {
			case MakeTarget::UNPROCESSED:
			case MakeTarget::PROCESSING:
				// cannot happen
				break;
			case MakeTarget::UP_TO_DATE:
				break;
			case MakeTarget::NEEDS_UPDATE:
				dependencyUpdated = true;
				break;
			case MakeTarget::MISSING:
			case MakeTarget::SKIPPING:
				if (!dependency->GetTarget()->IsIgnoreIfMissing())
					cantBuild = true;
				break;
		}
	}

	// For depends-on-leaves targets consider only the leaf times.
	if (target->DependsOnLeaves())
		newestDependencyTime = newestLeafTime;

	// decide what to do with the target
	Time time = makeTarget->GetTime();
	MakeTarget::State state;

	if (cantBuild) {
		state = MakeTarget::SKIPPING;
		if (!time.IsValid())
			time = Time(0);
	} else if (!isPseudoTarget && makeTarget->FileExists()) {
		if (!target->IsDontUpdate()
			&& (dependencyUpdated || newestDependencyTime > time)) {
			state = MakeTarget::NEEDS_UPDATE;
			time = std::max(time, newestDependencyTime);
		} else
			state = MakeTarget::UP_TO_DATE;
	} else {
		state = MakeTarget::NEEDS_UPDATE;
// TODO: Set to NEEDS_UPDATE also for pseudo-targets without actions?
		if (isPseudoTarget)
			time = newestDependencyTime;
		else
			time = fNow;
	}

	makeTarget->SetState(state);
	makeTarget->SetTime(time);
	makeTarget->SetLeafTime(makeTarget->IsLeaf() ? time : newestLeafTime);

// TODO: Support:
// - BUILD_ALWAYS
// - IGNORE_IF_MISSING
// - TEMPORARY
}


}	// namespace make
}	// namespace ham
