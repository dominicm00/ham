/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Copyright 2022, Dominic Martinez, dom@dominicm.dev.
 * Distributed under the terms of the MIT License.
 */

#include "make/Processor.hpp"

#include "behavior/Compatibility.hpp"
#include "code/Block.hpp"
#include "code/BuiltInRules.hpp"
#include "code/Constant.hpp"
#include "code/EvaluationContext.hpp"
#include "code/FunctionCall.hpp"
#include "code/Leaf.hpp"
#include "code/OnExpression.hpp"
#include "data/RegExp.hpp"
#include "data/RuleActions.hpp"
#include "data/StringBuffer.hpp"
#include "data/StringList.hpp"
#include "data/TargetBinder.hpp"
#include "data/TargetContainers.hpp"
#include "data/VariableDomain.hpp"
#include "make/Command.hpp"
#include "make/MakeException.hpp"
#include "make/MakeTarget.hpp"
#include "make/Piecemeal.hpp"
#include "make/TargetBuildInfo.hpp"
#include "make/TargetBuilder.hpp"
#include "parser/Parser.hpp"
#include "ruleset/HamRuleset.hpp"
#include "ruleset/JamRuleset.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <sstream>
#include <stdarg.h>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

namespace ham::make
{

using data::Time;
using std::unique_ptr;

static const String kHeaderScanVariableName("HDRSCAN");
static const String kHeaderRuleVariableName("HDRRULE");
static const String kJamShellVariableName("JAMSHELL");
static const String kTargetVariableName("JAM_TARGETS");

// TODO: This should be determined dynamically
static const size_t kMaxCommandLength = 8'000;

Processor::Processor()
	: fGlobalVariables(),
	  fTargets(),
	  fEvaluationContext(fGlobalVariables, fTargets),
	  fOptions(),
	  fPrimaryTargets(),
	  fMakeTargets(),
	  fMakeLevel(0),
	  fMakableTargets(),
	  fCommands(),
	  fTargetBuildInfos(),
	  fTargetsToUpdateCount(0)
{
	code::BuiltInRules::RegisterRules(fEvaluationContext.Rules());
}

Processor::~Processor()
{
	for (TargetBuildInfoSet::iterator it = fTargetBuildInfos.begin();
		 it != fTargetBuildInfos.end();
		 ++it) {
		delete *it;
	}

	for (MakeTargetMap::iterator it = fMakeTargets.begin();
		 it != fMakeTargets.end();
		 ++it) {
		delete it->second;
	}

	for (auto& targetCommands : fCommands) {
		for (auto command : targetCommands.second) {
			delete command;
		}
	}
}

void
Processor::SetOptions(const Options& options)
{
	fOptions = options;
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
	fGlobalVariables.Set(kTargetVariableName, targets);
}

void
Processor::SetForceUpdateTargets(const StringList& /* targets */)
{
	// TODO:...
}

void
Processor::ProcessRuleset()
{
	// parse code
	parser::Parser parser;

	util::Reference<code::Block> block;

	if (fOptions.RulesetFile().IsEmpty()) {
		parser.SetFileName("InternalRuleset");

		// Choose ruleset based on compatibility
		std::string ruleset;
		switch (fEvaluationContext.GetCompatibility()) {
			case behavior::COMPATIBILITY_BOOST_JAM:
				// TODO: Add Boost Jam's ruleset!
			case behavior::COMPATIBILITY_JAM:
				ruleset = ruleset::kJamRuleset;
				break;
			case behavior::COMPATIBILITY_HAM:
				ruleset = ruleset::kHamRuleset;
				break;
			default:
				throw MakeException("Unknown compatibility mode");
		}

		block.SetTo(parser.Parse(ruleset), true);
	} else {
		parser.SetFileName(fOptions.RulesetFile().ToStlString());
		block.SetTo(parser.ParseFile(fOptions.RulesetFile().ToCString()), true);
	}

	// execute the code
	block->Evaluate(fEvaluationContext);
}

void
Processor::PrepareTargets()
{
	fNow = Time::Now();
	// TODO: Not used yet!

	// Create make targets for the given primary target names.
	const StringList& primaryTargetNames =
		fGlobalVariables.LookupOrCreate(kTargetVariableName);
	if (primaryTargetNames.IsEmpty())
		throw MakeException("No targets specified");

	size_t primaryTargetCount = primaryTargetNames.Size();
	for (size_t i = 0; i < primaryTargetCount; i++) {
		String targetName = primaryTargetNames.ElementAt(i);
		Target* target = fTargets.Lookup(targetName);
		if (target == nullptr) {
			throw MakeException(
				std::string("Unknown target \"") + targetName.ToCString() + "\""
			);
		}

		_GetMakeTarget(target, true);
	}

	// Bind the targets and their dependencies recursively and decide their
	// fate tentatively -- e.g. for temporary targets a second pass is needed.
	fMakeLevel = 0;

	for (size_t i = 0; i < primaryTargetCount; i++) {
		String targetName = primaryTargetNames.ElementAt(i);
		Target* target = fTargets.Lookup(targetName);
		MakeTarget* makeTarget = _GetMakeTarget(target, false);
		fPrimaryTargets.Append(makeTarget);
		_PrepareTargetRecursively(makeTarget);
	}

	// Decide the targets' fate for good.
	// Reset the processing state first.
	for (MakeTargetMap::const_iterator it = fMakeTargets.begin();
		 it != fMakeTargets.end();
		 ++it) {
		it->second->SetProcessingState(MakeTarget::UNPROCESSED);
	}

	fMakeLevel = 0;

	for (MakeTargetSet::Iterator it = fPrimaryTargets.GetIterator();
		 it.HasNext();) {
		MakeTarget* makeTarget = it.Next();
		_SealTargetFateRecursively(makeTarget, Time::MIN, true);
	}
}

void
Processor::BuildTargets()
{
	printf("...found %zu target(s)...\n", fMakeTargets.size());

	for (MakeTargetSet::Iterator it = fPrimaryTargets.GetIterator();
		 it.HasNext();) {
		_CollectMakableTargets(it.Next());
	}

	if (fMakableTargets.IsEmpty())
		return;

	if (fTargetsToUpdateCount > 0)
		printf("...updating %zu target(s)...\n", fTargetsToUpdateCount);

	// get the JAMSHELL variable
	//
	// TODO: This should be target-local!
	StringList jamShell(fEvaluationContext.LookupVariable(kJamShellVariableName)
	);
	if (!jamShell.IsTrue()) {
		jamShell.Append(String("/bin/sh"));
		jamShell.Append(String("-c"));
		jamShell.Append(String("%"));
		// TODO: Platform dependent!
	}

	TargetBuilder builder(fOptions, jamShell);

	size_t targetsUpdated = 0;
	size_t targetsFailed = 0;
	size_t targetsSkipped = 0;

	while (!fMakableTargets.IsEmpty() || builder.HasPendingBuildInfos()) {
		while (TargetBuildInfo* buildInfo = builder.NextFinishedBuildInfo(
				   !builder.HasSpareJobSlots() || fMakableTargets.IsEmpty()
			   )) {
			if (buildInfo->HasFailed()) {
				targetsFailed++;
				targetsSkipped +=
					_TargetMade(buildInfo->GetTarget(), MakeTarget::FAILED);
			} else {
				targetsUpdated++;
				targetsSkipped +=
					_TargetMade(buildInfo->GetTarget(), MakeTarget::DONE);
			}
		}

		while (builder.HasSpareJobSlots() && !fMakableTargets.IsEmpty()) {
			MakeTarget* makeTarget = fMakableTargets.Head();
			fMakableTargets.RemoveAt(0);
			if (TargetBuildInfo* buildInfo = _MakeTarget(makeTarget))
				builder.AddBuildInfo(buildInfo);
		}
	}

	// Delete temporaries
	std::uintmax_t filesRemoved = 0;
	for (MakeTargetSet::Iterator it = fTemporaryTargets.GetIterator();
		 it.HasNext();) {
		MakeTarget* makeTarget = it.Next();
		const std::filesystem::path path{makeTarget->BoundPath().ToStlString()};
		filesRemoved += std::filesystem::remove_all(path);
	}

	if (filesRemoved > 0) {
		std::cout << "...removed " << filesRemoved << " temporary files..."
				  << std::endl;
	}

	if (targetsFailed > 0)
		printf("...failed updating %zu target(s)...\n", targetsFailed);
	if (targetsSkipped > 0)
		printf("...skipped %zu target(s)...\n", targetsSkipped);
	if (targetsUpdated > 0)
		printf("...updated %zu target(s)...\n", targetsUpdated);
}

MakeTarget*
Processor::_GetMakeTarget(Target* target, bool create)
{
	MakeTargetMap::iterator it = fMakeTargets.find(target);
	if (it != fMakeTargets.end())
		return it->second;

	if (!create)
		return nullptr;

	unique_ptr<MakeTarget> makeTarget(new MakeTarget(target));
	fMakeTargets[target] = makeTarget.get();
	return makeTarget.release();
}

MakeTarget*
Processor::_GetMakeTarget(const String& targetName, bool create)
{
	Target* target = create ? fTargets.LookupOrCreate(targetName)
							: fTargets.Lookup(targetName);
	return target != nullptr ? _GetMakeTarget(target, create) : nullptr;
}

bool
Processor::_IsPseudoTarget(const MakeTarget* makeTarget) const
{
	// Like Jam we also consider missing targets without actions but with
	// dependencies pseudo targets, even if they haven't been declared NotFile.
	// TODO: This should be made Jam compatibility behavior and disabled by
	// default. It probably just complicates trouble shooting for the user to
	// hide this error.
	const Target* target = makeTarget->GetTarget();
	return target->IsNotAFile()
		|| (!makeTarget->FileExists() && !target->HasActionsCalls()
			&& !target->Dependencies().IsEmpty());
}

bool
Processor::_IsMakeableTarget(const MakeTarget* makeTarget) const
{
	return makeTarget->GetTarget()->IsBuildAlways()
		|| !_IsPseudoTarget(makeTarget);
}

void
Processor::_PrepareTargetRecursively(MakeTarget* makeTarget)
{
	// Check whether the target has already been processed (also detect cycles)
	// and mark in-progress.
	if (makeTarget->GetProcessingState() != MakeTarget::UNPROCESSED) {
		if (makeTarget->GetProcessingState() == MakeTarget::PROCESSING) {
			throw MakeException(
				std::string("Target \"") + makeTarget->Name().ToCString()
				+ "\" depends on itself"
			);
		}

		// already done
		return;
	}

	makeTarget->SetProcessingState(MakeTarget::PROCESSING);

	// bind the target
	_BindTarget(makeTarget);

	// Determine whether it is a pseudo target.
	bool isPseudoTarget = _IsPseudoTarget(makeTarget);
	if (isPseudoTarget || !makeTarget->FileExists())
		makeTarget->SetOriginalTime(Time::MIN);

	Time time = makeTarget->GetOriginalTime();
	if (!time.IsValid())
		time = Time::MIN;

	// add make targets for dependencies
	const Target* target = makeTarget->GetTarget();
	const TargetSet& dependencies = target->Dependencies();
	for (TargetSet::Iterator it = dependencies.GetIterator(); it.HasNext();)
		makeTarget->AddDependency(_GetMakeTarget(it.Next(), true));

	// recursively process the target's dependencies
	Time newestDependencyTime = Time::MIN;
	Time newestLeafTime = Time::MIN;
	bool dependencyUpdated = false;
	bool cantMake = false;
	for (size_t i = 0; i < makeTarget->Dependencies().Size(); i++) {
		MakeTarget* dependency = makeTarget->Dependencies().ElementAt(i);
		dependency->AddParent(makeTarget);

		fMakeLevel++;
		_PrepareTargetRecursively(dependency);
		fMakeLevel--;

		// Add the dependency's includes as the target's dependencies. This
		// will also take care of recursive includes in a breadth first manner
		// as we keep appending the newly found includes at the end of our
		// dependency list.
		makeTarget->AddDependencies(dependency->Includes());

		// track times
		newestDependencyTime =
			std::max(newestDependencyTime, dependency->GetTime());
		newestLeafTime = std::max(newestLeafTime, dependency->LeafTime());

		switch (dependency->GetFate()) {
			case MakeTarget::KEEP:
				break;
			case MakeTarget::MAKE_IF_NEEDED:
				break;
			case MakeTarget::MAKE:
				if (_IsMakeableTarget(dependency)
					&& (!target->DependsOnLeaves() || dependency->IsLeaf())) {
					dependencyUpdated = true;
				}
				break;
			case MakeTarget::CANT_MAKE:
				cantMake = true;
				break;
		}
	}

	// header scanning
	if (makeTarget->FileExists())
		_ScanForHeaders(makeTarget);

	// add make targets for includes
	const TargetSet& includes = target->Includes();
	for (TargetSet::Iterator it = includes.GetIterator(); it.HasNext();)
		makeTarget->AddInclude(_GetMakeTarget(it.Next(), true));

	// For depends-on-leaves targets consider only the leaf times.
	if (target->DependsOnLeaves())
		newestDependencyTime = newestLeafTime;

	// Consider a "don't update" target very old, so targets depending on it
	// won't be remade unnecessarily. Forced updates take precedence over "don't
	// update"
	if (target->IsDontUpdate() && !target->IsBuildAlways()) {
		time = newestDependencyTime = Time::MIN;
		makeTarget->SetOriginalTime(time);
	}

	// determine the target's state and fate
	MakeTarget::State state;
	MakeTarget::Fate fate = MakeTarget::KEEP;
	if (isPseudoTarget || !makeTarget->FileExists()) {
		state = MakeTarget::MISSING;
		fate = MakeTarget::MAKE;
	} else if (newestDependencyTime > time) {
		state = MakeTarget::OUT_OF_DATE;
		if (!target->IsDontUpdate())
			fate = MakeTarget::MAKE;
	} else {
		state = MakeTarget::UP_TO_DATE;
		if (dependencyUpdated)
			fate = MakeTarget::MAKE;
	}

	if (target->IsBuildAlways())
		fate = MakeTarget::MAKE;

	if (fate == MakeTarget::MAKE && cantMake)
		fate = MakeTarget::CANT_MAKE;

	if (fate == MakeTarget::MAKE) {
		if (target->IsTemporary()) {
			if (target->IsBuildAlways()) {
				// We know the temporary will be built, so add it to the temp
				// target list
				fTemporaryTargets.Append(makeTarget);
			} else {
				// If target is temporary and not forced, downgrade to
				// MAKE_IF_NEEDED.
				fate = MakeTarget::MAKE_IF_NEEDED;
			}
		}

		if (!target->HasActionsCalls() && !isPseudoTarget) {
			if (target->IsIgnoreIfMissing()) {
				fate = MakeTarget::KEEP;
			} else {
				std::stringstream warning{};
				warning << "don't know how to make " << target->Name();
				_PrintWarning(warning.str());
				fate = MakeTarget::CANT_MAKE;
			}
		}
	}

	time = std::max(time, newestDependencyTime);
	makeTarget->SetState(state);
	makeTarget->SetFate(fate);
	makeTarget->SetTime(time);
	makeTarget->SetLeafTime(makeTarget->IsLeaf() ? time : newestLeafTime);
	makeTarget->SetProcessingState(MakeTarget::PROCESSED);

	// TODO: Support:
	// - BUILD_ALWAYS
	// - IGNORE_IF_MISSING
	// - TEMPORARY
}

void
Processor::_SealTargetFateRecursively(
	MakeTarget* makeTarget,
	data::Time parentTime,
	bool makeParent
)
{
	// Check whether the target has already been processed (also detect cycles)
	// and mark in-progress.
	if (makeTarget->GetProcessingState() != MakeTarget::UNPROCESSED) {
		if (makeTarget->GetProcessingState() == MakeTarget::PROCESSING) {
			throw MakeException(
				std::string("Target \"") + makeTarget->Name().ToCString()
				+ "\" depends on itself"
			);
		}

		// Already done, though we process it again, if its fate will change.
		if (makeTarget->GetFate() != MakeTarget::MAKE_IF_NEEDED || !makeParent)
			return;
	}

	makeTarget->SetProcessingState(MakeTarget::PROCESSING);

	if (makeTarget->GetFate() == MakeTarget::MAKE_IF_NEEDED && makeParent) {
		makeTarget->SetFate(MakeTarget::MAKE);
		// Add upgraded targets to temp list
		fTemporaryTargets.Append(makeTarget);
	}

	if (fOptions.IsPrintMakeTree()) {
		_PrintMakeTreeStep(makeTarget, "make", nullptr, nullptr);
		_PrintMakeTreeBinding(makeTarget);
	}

	for (size_t i = 0; i < makeTarget->Dependencies().Size(); i++) {
		MakeTarget* dependency = makeTarget->Dependencies().ElementAt(i);
		fMakeLevel++;
		_SealTargetFateRecursively(
			dependency,
			makeTarget->GetOriginalTime(),
			makeTarget->GetFate() == MakeTarget::MAKE
				&& _IsMakeableTarget(makeTarget)
		);
		fMakeLevel--;
	}

	makeTarget->SetProcessingState(MakeTarget::PROCESSED);

	if (fOptions.IsPrintMakeTree())
		_PrintMakeTreeState(makeTarget, parentTime);
}

void
Processor::_BindTarget(MakeTarget* makeTarget)
{
	if (makeTarget->IsBound())
		return;

	const Target* target = makeTarget->GetTarget();

	String boundPath;
	data::FileStatus fileStatus;
	data::TargetBinder::Bind(
		*fEvaluationContext.GlobalVariables(),
		target,
		boundPath,
		fileStatus
	);
	makeTarget->SetBoundPath(boundPath);
	makeTarget->SetFileStatus(fileStatus);
}

void
Processor::_ScanForHeaders(MakeTarget* makeTarget)
{
	// Get the on-target HDRSCAN and HDRRULE variables.
	// Note: We're not getting the global variables, if the on-target ones
	// aren't defined, since it really doesn't make much sense to define them
	// globally.
	const Target* target = makeTarget->GetTarget();
	const data::VariableDomain* variables = target->Variables();
	if (variables == nullptr)
		return;

	const StringList* scanPattern = variables->Lookup(kHeaderScanVariableName);
	const StringList* scanRule = variables->Lookup(kHeaderRuleVariableName);
	if (scanPattern == nullptr || scanRule == nullptr || scanPattern->IsEmpty()
		|| scanRule->IsEmpty()) {
		// TODO: Warn if only one variable is set as it's probably a mistake!
		return;
	}

	// prepare the grep regular expression
	data::RegExp regExp(scanPattern->ElementAt(0).ToCString());

	// open the file
	std::ifstream file(makeTarget->BoundPath().ToCString());
	if (file.fail()) {
		// TODO: Error/warning!
		return;
	}

	// scan it
	StringList headersFound;
	std::string line;
	while (std::getline(file, line)) {
		data::RegExp::MatchResult result = regExp.Match(line.c_str());
		if (result.HasMatched()) {
			size_t groupCount = result.GroupCount();
			for (size_t i = 0; i < groupCount; i++) {
				size_t startOffset = result.GroupStartOffsetAt(i);
				size_t endOffset = result.GroupEndOffsetAt(i);
				String headerName(
					line.c_str() + startOffset,
					endOffset - startOffset
				);
				if (!headerName.IsEmpty())
					headersFound.Append(headerName);
			}
		}
	}

	file.close();

	// If anything was found, call the HDRRULE.
	if (!headersFound.IsEmpty()) {
		// Construct the code to evaluate the rule under the influence of the
		// target.
		code::NodeReference targetNameNode(
			new code::Constant(target->Name()),
			true
		);
		code::NodeReference headersNode(new code::Constant(headersFound), true);
		code::NodeReference callFunction(new code::Constant(scanRule), true);
		util::Reference<code::FunctionCall> call(
			new code::FunctionCall(callFunction.Get()),
			true
		);
		call->AddArgument(targetNameNode.Get());
		call->AddArgument(headersNode.Get());
		code::NodeReference onExpression(
			new code::OnExpression(targetNameNode.Get(), call.Get()),
			true
		);
		onExpression->Evaluate(fEvaluationContext);
	}
}

bool
Processor::_CollectMakableTargets(MakeTarget* makeTarget)
{
	bool needToMake = false;
	switch (makeTarget->GetFate()) {
		case MakeTarget::MAKE:
			makeTarget->SetMakeState(MakeTarget::PENDING);
			if (_IsMakeableTarget(makeTarget))
				fTargetsToUpdateCount++;
			needToMake = true;
			break;
		case MakeTarget::MAKE_IF_NEEDED:
			// If it is still MAKE_IF_NEEDED after the second pass, we don't
			// need to make it.
		case MakeTarget::KEEP:
			makeTarget->SetMakeState(MakeTarget::DONE);
			break;
		case MakeTarget::CANT_MAKE:
			std::stringstream warning{};
			warning << "can't make " << makeTarget->GetTarget()->Name()
					<< ", skipping";
			_PrintWarning(warning.str());
			makeTarget->SetMakeState(MakeTarget::SKIPPED);
			break;
	}

	size_t pendingDependencyCount = 0;
	for (MakeTargetSet::Iterator it = makeTarget->Dependencies().GetIterator();
		 it.HasNext();) {
		if (_CollectMakableTargets(it.Next()))
			pendingDependencyCount++;
	}

	makeTarget->SetPendingDependenciesCount(pendingDependencyCount);

	if (pendingDependencyCount == 0 && needToMake)
		fMakableTargets.Append(makeTarget);

	return needToMake;
}

CommandList
Processor::_MakeCommands(Target* target)
{
	// Check command cache
	if (auto it = fCommands.find(target); it != fCommands.end())
		return it->second;

	// TODO: Support RuleActions::PIECEMEAL
	//
	// Each TOGETHER action can be associated to a set of sources (the targets
	// are implied since TOGETHER actions can only have one target). Ham does
	// not guarantee that TOGETHER actions have sources in any order, so we
	// don't need to use a SequentialSet.
	using TogetherCallMap = std::map<data::RuleActions*, std::set<Target*>>;
	TogetherCallMap togetherMap{};
	CommandList& commandList = fCommands[target];

	for (std::vector<data::RuleActionsCall*>::const_iterator it =
			 target->ActionsCalls().begin();
		 it != target->ActionsCalls().end();
		 ++it) {
		data::RuleActionsCall* actionsCall = *it;
		data::RuleActions* actions = actionsCall->Actions();

		if (actions->IsTogether()) {
			// Together actions cannot have multiple targets (ADR 6)
			if (auto numTargets = actionsCall->Targets().size();
				numTargets > 1) {
				std::stringstream error{};
				error << "Error: Action " << actions->RuleName()
					  << " has 'together' modifier and must be passed exactly "
						 "1 target, "
						 "but was passed "
					  << numTargets;
				throw MakeException(error.str());
			}

			for (auto source : actionsCall->SourceTargets())
				togetherMap[actions].insert(source);
		} else {
			_BuildCommands(actionsCall, commandList);
		}
	}

	// Add TOGETHER actions
	for (auto [action, sourceSet] : togetherMap) {
		data::TargetList targets{target};
		data::TargetList sources(sourceSet.begin(), sourceSet.end());
		auto actionsCall = new data::RuleActionsCall{action, targets, sources};
		// Target takes ownership of actions call
		target->AddActionsCall(actionsCall);
		_BuildCommands(actionsCall, commandList);
	}

	return commandList;
}

TargetBuildInfo*
Processor::_MakeTarget(MakeTarget* makeTarget)
{
	Target* target = makeTarget->GetTarget();
	if (!_IsMakeableTarget(makeTarget) || target->ActionsCalls().empty()) {
		_TargetMade(makeTarget, MakeTarget::DONE);
		return nullptr;
	}

	unique_ptr<TargetBuildInfo> buildInfo(new TargetBuildInfo(makeTarget));

	auto commands = _MakeCommands(target);
	for (auto command : commands) {
		if (command != nullptr)
			buildInfo->AddCommand(command);
	}

	return buildInfo.release();
}

size_t
Processor::_TargetMade(MakeTarget* makeTarget, MakeTarget::MakeState state)
{
	if (makeTarget->GetMakeState() == MakeTarget::PENDING)
		makeTarget->SetMakeState(state);

	size_t skippedCount = 0;

	switch (state) {
		case MakeTarget::DONE:
			break;
		case MakeTarget::PENDING:
			// cannot happen
			// TODO: Have an internal error here?
			break;
		case MakeTarget::FAILED:
			break;
		case MakeTarget::SKIPPED: {
			// get the first dependency that couldn't be made
			MakeTarget* lackingDependency = nullptr;
			for (MakeTargetSet::Iterator it =
					 makeTarget->Dependencies().GetIterator();
				 it.HasNext();) {
				MakeTarget* dependency = it.Next();
				if (dependency->GetMakeState() == MakeTarget::FAILED
					|| dependency->GetMakeState() == MakeTarget::SKIPPED) {
					lackingDependency = dependency;
				}
			}

			printf(
				"...skipped %s for lack of %s...\n",
				makeTarget->Name().ToCString(),
				lackingDependency != nullptr
					? lackingDependency->Name().ToCString()
					: "???"
			);
			skippedCount++;
			break;
		}
	}

	// propagate the event to the target's parents
	for (MakeTargetSet::Iterator it = makeTarget->Parents().GetIterator();
		 it.HasNext();) {
		MakeTarget* parent = it.Next();
		size_t pendingDependencyCount = parent->PendingDependenciesCount() - 1;
		parent->SetPendingDependenciesCount(pendingDependencyCount);

		if (state != MakeTarget::DONE)
			parent->SetMakeState(MakeTarget::SKIPPED);

		if (pendingDependencyCount == 0) {
			if (parent->GetMakeState() == MakeTarget::PENDING)
				fMakableTargets.Insert(parent, 0);
			else
				skippedCount += _TargetMade(parent, parent->GetMakeState());
		}
	}

	return skippedCount;
}

void
Processor::_BindActionsTargets(
	data::RuleActionsCall* actionsCall,
	bool isSources,
	StringList& boundTargets
)
{
	const data::RuleActions* actions = actionsCall->Actions();
	const bool isExistingAction = actions->IsExisting();
	const bool isUpdatedAction = actions->IsUpdated();

	data::TargetList targetList =
		isSources ? actionsCall->SourceTargets() : actionsCall->Targets();

	Target* primaryTarget = *actionsCall->Targets().begin();
	MakeTarget* primaryMakeTarget = _GetMakeTarget(primaryTarget, true);

	for (const auto target : targetList) {
		MakeTarget* makeTarget = _GetMakeTarget(target, true);

		if (!makeTarget->IsBound()) {
			// Bind independent targets, but don't make them.
			_BindTarget(makeTarget);

			// Sources to EXISTING actions are always independent, so
			// don't warn about them.
			if (!(isSources && isExistingAction)) {
				std::stringstream warning{};
				auto warningString{
					_IsPseudoTarget(makeTarget)
						? "using independent pseudotarget "
						: "using independent target "};
				warning << warningString << makeTarget->GetTarget()->Name();

				// Sources to UPDATED actions must be in the dependency
				// tree
				if (isSources && isUpdatedAction)
					warning << " in an 'updated' action";

				bool errorOnIndependentUpdated =
					fEvaluationContext.GetBehavior().GetErrorUpdatedSource()
					== behavior::Behavior::ERROR_INDEPENDENT_UPDATED;

				if (isSources && isUpdatedAction && errorOnIndependentUpdated) {
					throw MakeException(warning.str());
				} else {
					_PrintWarning(warning.str());
				}
			}
		}

		if (isSources) {
			if (isExistingAction && !makeTarget->FileExists())
				continue;

			// A source is updated if:
			// - It is being made, or
			// - It is newer than the primary target, and
			// - It is makeable
			bool isMake = makeTarget->GetFate() == MakeTarget::MAKE;
			bool isNewer =
				primaryMakeTarget->GetOriginalTime() < makeTarget->GetTime();
			bool isUpdatedTarget =
				_IsMakeableTarget(makeTarget) && (isMake || isNewer);

			if (isUpdatedAction && !isUpdatedTarget)
				continue;
		}

		boundTargets.Append(makeTarget->BoundPath());
	}
}

void
Processor::_BuildCommands(
	data::RuleActionsCall* actionsCall,
	CommandList& commands
)
{
	const data::RuleActions* actions = actionsCall->Actions();
	const bool isExistingAction = actions->IsExisting();
	const bool isUpdatedAction = actions->IsUpdated();

	auto numTargets = actionsCall->Targets().size();

	// Actions must have at least one target (ADR 5)
	//
	// TODO: This is a redundant check because there shouldn't be a way to
	// actually call an action without a target. This needs to be implemented
	// somewhere in parsing.
	if (numTargets == 0) {
		std::stringstream error{};
		error << "Error: Action " << actions->RuleName()
			  << " was called with no targets";
		throw MakeException(error.str());
	}

	// Updated actions cannot have multiple targets (ADR 4)
	if (isUpdatedAction && numTargets > 1) {
		std::stringstream error{};
		error << "Error: Action " << actions->RuleName()
			  << " has 'updated' modifier and must be passed exactly 1 target, "
				 "but was passed "
			  << numTargets;
		throw MakeException(error.str());
	}

	// create a variable domain for the built-in variables (the numbered ones
	// and "<" and ">")
	data::VariableDomain builtInVariables;

	StringList boundSourceTargets;
	_BindActionsTargets(actionsCall, true, boundSourceTargets);
	// If sources are being expanded and have been trimmed to empty by
	// EXISTING or UPDATED, cancel this action.
	if (boundSourceTargets.IsEmpty() && (isExistingAction || isUpdatedAction)) {
		return;
	}

	/* Bind non-source variables */

	// Bind targets
	StringList boundTargets;
	_BindActionsTargets(actionsCall, false, boundTargets);
	builtInVariables.Set("1", boundTargets);
	builtInVariables.Set("<", boundTargets);

	// Push a copy of the primary target's variable domain as a new local scope
	const Target* primaryTarget = *actionsCall->Targets().begin();
	data::VariableDomain localVariables;
	if (primaryTarget->Variables() != nullptr)
		localVariables = *primaryTarget->Variables();
	data::VariableScope* oldLocalScope = fEvaluationContext.LocalScope();
	data::VariableScope localScope(localVariables, oldLocalScope);

	// set the local variable scope and the built-in variables
	fEvaluationContext.SetLocalScope(&localScope);

	data::VariableDomain* oldBuiltInVariables =
		fEvaluationContext.BuiltInVariables();
	fEvaluationContext.SetBuiltInVariables(&builtInVariables);

	// bind the variables specified by the actions
	for (StringList::Iterator it = actionsCall->Actions()->Variables();
		 it.HasNext();) {
		String variable = it.Next();
		if (const StringList* values =
				fEvaluationContext.LookupVariable(variable)) {
			StringList newValues;
			for (StringList::Iterator valueIt = values->GetIterator();
				 valueIt.HasNext();) {
				MakeTarget* makeTarget = _GetMakeTarget(valueIt.Next(), true);
				_BindTarget(makeTarget);
				newValues.Append(makeTarget->BoundPath());
			}
			localVariables.Set(variable, newValues);
		}
	}

	// Split command into words. Each word is a pair consisting of the string
	// and trailing whitespace.
	std::vector<std::pair<std::string_view, std::string_view>> words{};
	String rawCommandLine = actionsCall->Actions()->Actions();
	const char* remainder = rawCommandLine.ToCString();
	const char* end = remainder + rawCommandLine.Length();
	const char* wordStart = nullptr;
	const char* wordEnd = nullptr;
	while (remainder < end) {
		const bool isSpace = std::isspace(*remainder);

		if (!isSpace && wordStart == nullptr)
			wordStart = remainder;
		if (isSpace && wordEnd == nullptr && wordStart != nullptr)
			wordEnd = remainder;
		if (!isSpace && wordEnd != nullptr) {
			words.push_back({{wordStart, wordEnd}, {wordEnd, remainder}});
			wordStart = remainder;
			wordEnd = nullptr;
		}

		remainder++;
	}
	if (wordEnd == nullptr)
		wordEnd = remainder - 1;
	words.push_back({{wordStart, wordEnd}, {wordEnd, remainder - 1}});

	data::StringListList sources{};
	if (actions->IsPiecemeal() && !boundSourceTargets.IsEmpty()) {
		std::uint32_t maxLine =
			actions->MaxLine() > 0 ? actions->MaxLine() : kMaxCommandLength;

		sources = Piecemeal::Words(
			fEvaluationContext,
			actions->RuleName().ToStlString(),
			words,
			boundSourceTargets,
			maxLine
		);
	} else {
		sources.push_back(boundSourceTargets);
	}

	for (StringList commandSources : sources) {
		data::VariableDomain builtInWithSources{builtInVariables};
		builtInWithSources.Set("2", commandSources);
		builtInWithSources.Set(">", commandSources);
		fEvaluationContext.SetBuiltInVariables(&builtInWithSources);

		// Build command
		data::String commandLine{};
		for (auto& [word, space] : words) {
			auto evaluatedWord = code::Leaf::EvaluateString(
				fEvaluationContext,
				word.cbegin(),
				word.cend(),
				nullptr
			);
			const StringPart separator{" "};

			commandLine = commandLine + evaluatedWord.Join(separator)
				+ std::string{space}.c_str();
		}
		commands.push_back(new Command(
			actionsCall,
			std::move(commandLine),
			std::move(boundTargets)
		));
	}

	// reinstate the old local variable scope and the built-in variables
	fEvaluationContext.SetLocalScope(oldLocalScope);
	fEvaluationContext.SetBuiltInVariables(oldBuiltInVariables);
	return;
}

void
Processor::_PrintMakeTreeBinding(const MakeTarget* makeTarget)
{
	const char* timeString;
	if (makeTarget->FileExists()) {
		_PrintMakeTreeStep(
			makeTarget,
			"bind",
			nullptr,
			": %s",
			makeTarget->BoundPath().ToCString()
		);
		timeString = makeTarget->GetOriginalTime().ToString().ToCString();
	} else {
		if (_IsPseudoTarget(makeTarget))
			timeString = "unbound";
		else
			timeString = "missing";
	}

	// TODO: In Jam binding might also be "parent".
	_PrintMakeTreeStep(makeTarget, "time", nullptr, ": %s", timeString);
}

void
Processor::_PrintMakeTreeState(
	const MakeTarget* makeTarget,
	data::Time parentTime
)
{
	const Target* target = makeTarget->GetTarget();
	const char* stateString;
	char madeString[6] = "made ";
	char& flag = madeString[4];

	if (makeTarget->FileExists())
		stateString = nullptr;
	else if (_IsPseudoTarget(makeTarget))
		stateString = "pseudo";
	else
		stateString = "missing";

	switch (makeTarget->GetFate()) {
		case MakeTarget::MAKE:
			if (stateString == nullptr)
				stateString = "update";
			if (!_IsPseudoTarget(makeTarget) && target->HasActionsCalls())
				flag = '+';
			break;
		case MakeTarget::MAKE_IF_NEEDED:
			if (stateString == nullptr)
				stateString = "no update";
			flag = '-';
			break;
		case MakeTarget::KEEP:
			if (makeTarget->GetOriginalTime() > parentTime) {
				flag = '*';
				stateString = "newer";
			} else
				stateString = "stable";
			break;
		case MakeTarget::CANT_MAKE:
			stateString = "missing!";
			break;
	}

	_PrintMakeTreeStep(makeTarget, madeString, stateString, nullptr);
}

void
Processor::_PrintMakeTreeStep(
	const MakeTarget* makeTarget,
	const char* step,
	const char* state,
	const char* pattern,
	...
)
{
	printf(
		"%-7s %-10s %*s%s",
		step,
		state != nullptr ? state : "--",
		fMakeLevel,
		"",
		makeTarget->Name().ToCString()
	);
	if (pattern != nullptr) {
		char buffer[1024];
		va_list args;
		va_start(args, pattern);
		vsnprintf(buffer, sizeof(buffer), pattern, args);
		va_end(args);
		printf("%s", buffer);
	}

	printf("\n");
}

void
Processor::_PrintWarning(std::string warning)
{
	std::cerr << "...warning: " << warning << "..." << std::endl;
}

} // namespace ham::make
