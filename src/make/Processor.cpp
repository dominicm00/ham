/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "make/Processor.hpp"

#include "behavior/Compatibility.hpp"
#include "code/Block.hpp"
#include "code/BuiltInRules.hpp"
#include "code/Constant.hpp"
#include "code/FunctionCall.hpp"
#include "code/Leaf.hpp"
#include "code/OnExpression.hpp"
#include "data/RegExp.hpp"
#include "data/TargetBinder.hpp"
#include "make/Command.hpp"
#include "make/MakeException.hpp"
#include "make/MakeTarget.hpp"
#include "make/TargetBuildInfo.hpp"
#include "make/TargetBuilder.hpp"
#include "parser/Parser.hpp"
#include "ruleset/HamRuleset.hpp"
#include "ruleset/JamRuleset.hpp"

#include <fstream>
#include <memory>
#include <stdarg.h>

namespace ham::make
{

using data::Time;

static const String kHeaderScanVariableName("HDRSCAN");
static const String kHeaderRuleVariableName("HDRRULE");
static const String kJamShellVariableName("JAMSHELL");

Processor::Processor()
	: fGlobalVariables(),
	  fTargets(),
	  fEvaluationContext(fGlobalVariables, fTargets),
	  fOptions(),
	  fPrimaryTargetNames(),
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

	for (CommandMap::iterator it = fCommands.begin(); it != fCommands.end();
		 ++it) {
		delete it->second;
	}

	for (MakeTargetMap::iterator it = fMakeTargets.begin();
		 it != fMakeTargets.end();
		 ++it) {
		delete it->second;
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
	fPrimaryTargetNames = targets;
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
	if (fPrimaryTargetNames.IsEmpty())
		throw MakeException("No targets specified");

	size_t primaryTargetCount = fPrimaryTargetNames.Size();
	for (size_t i = 0; i < primaryTargetCount; i++) {
		String targetName = fPrimaryTargetNames.ElementAt(i);
		const Target* target = fTargets.Lookup(targetName);
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
		String targetName = fPrimaryTargetNames.ElementAt(i);
		const Target* target = fTargets.Lookup(targetName);
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
		_SealTargetFateRecursively(makeTarget, Time(0), true);
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

	if (targetsFailed > 0)
		printf("...failed updating %zu target(s)...\n", targetsFailed);
	if (targetsSkipped > 0)
		printf("...skipped %zu target(s)...\n", targetsSkipped);
	if (targetsUpdated > 0)
		printf("...updated %zu target(s)...\n", targetsUpdated);
}

MakeTarget*
Processor::_GetMakeTarget(const Target* target, bool create)
{
	MakeTargetMap::iterator it = fMakeTargets.find(target);
	if (it != fMakeTargets.end())
		return it->second;

	if (!create)
		return nullptr;

	std::unique_ptr<MakeTarget> makeTarget(new MakeTarget(target));
	fMakeTargets[target] = makeTarget.get();
	return makeTarget.release();
}

MakeTarget*
Processor::_GetMakeTarget(const String& targetName, bool create)
{
	const Target* target = create ? fTargets.LookupOrCreate(targetName)
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
		makeTarget->SetOriginalTime(Time(0));

	Time time = makeTarget->GetOriginalTime();
	if (!time.IsValid())
		time = Time(0);

	// add make targets for dependencies
	const Target* target = makeTarget->GetTarget();
	const TargetSet& dependencies = target->Dependencies();
	for (TargetSet::Iterator it = dependencies.GetIterator(); it.HasNext();)
		makeTarget->AddDependency(_GetMakeTarget(it.Next(), true));

	// recursively process the target's dependencies
	Time newestDependencyTime(0);
	Time newestLeafTime(0);
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
				if (!_IsPseudoTarget(dependency)
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
	// won't be remade unnecessarily.
	if (target->IsDontUpdate()) {
		time = newestDependencyTime = Time(0);
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

	if (fate == MakeTarget::MAKE && cantMake)
		fate = MakeTarget::CANT_MAKE;

	if (fate == MakeTarget::MAKE) {
		// If target is temporary, downgrade to MAKE_IF_NEEDED.
		if (target->IsTemporary())
			fate = MakeTarget::MAKE_IF_NEEDED;

		if (!target->HasActionsCalls() && !isPseudoTarget) {
			if (target->IsIgnoreIfMissing())
				fate = MakeTarget::KEEP;
			else
				fate = MakeTarget::CANT_MAKE;
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

	if (makeTarget->GetFate() == MakeTarget::MAKE_IF_NEEDED && makeParent)
		makeTarget->SetFate(MakeTarget::MAKE);

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
				&& !_IsPseudoTarget(makeTarget)
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
	if (!regExp.IsValid())
		return;

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
			if (!_IsPseudoTarget(makeTarget))
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

TargetBuildInfo*
Processor::_MakeTarget(MakeTarget* makeTarget)
{
	const Target* target = makeTarget->GetTarget();
	if (_IsPseudoTarget(makeTarget) || target->ActionsCalls().empty()) {
		_TargetMade(makeTarget, MakeTarget::DONE);
		return nullptr;
	}

	std::unique_ptr<TargetBuildInfo> buildInfo(new TargetBuildInfo(makeTarget));

	for (std::vector<data::RuleActionsCall*>::const_iterator it =
			 target->ActionsCalls().begin();
		 it != target->ActionsCalls().end();
		 ++it) {
		data::RuleActionsCall* actionsCall = *it;

		util::Reference<Command> command;
		CommandMap::iterator commandIt = fCommands.find(actionsCall);
		if (commandIt != fCommands.end()) {
			command.SetTo(commandIt->second);
		} else {
			// prepare the actions command line
			command.SetTo(_BuildCommand(actionsCall), true);
			if (actionsCall->Targets().size() > 1) {
				fCommands[actionsCall] = command.Get();
				command.Get()->AcquireReference();
			}
		}

		buildInfo->AddCommand(command.Get());
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

Command*
Processor::_BuildCommand(data::RuleActionsCall* actionsCall)
{
	// create a variable domain for the built-in variables (the numbered ones
	// and "<" and ">")
	data::VariableDomain builtInVariables;

	StringList boundTargets;
	for (data::TargetList::const_iterator it = actionsCall->Targets().begin();
		 it != actionsCall->Targets().end();
		 ++it) {
		MakeTarget* makeTarget = _GetMakeTarget(*it, true);
		boundTargets.Append(makeTarget->BoundPath());
	}
	builtInVariables.Set("1", boundTargets);
	builtInVariables.Set("<", boundTargets);

	StringList boundSourceTargets;
	for (data::TargetList::const_iterator it =
			 actionsCall->SourceTargets().begin();
		 it != actionsCall->SourceTargets().end();
		 ++it) {
		MakeTarget* makeTarget = _GetMakeTarget(*it, true);
		boundSourceTargets.Append(makeTarget->BoundPath());
	}
	builtInVariables.Set("2", boundSourceTargets);
	builtInVariables.Set(">", boundSourceTargets);

	// get the first of the targets and push a copy of its variable domain as a
	// new local scope
	const Target* target = *actionsCall->Targets().begin();
	data::VariableDomain localVariables;
	if (target->Variables() != nullptr)
		localVariables = *target->Variables();
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

	String rawCommandLine = actionsCall->Actions()->Actions();
	const char* remainder = rawCommandLine.ToCString();
	const char* end = remainder + rawCommandLine.Length();
	data::StringBuffer commandLine;
	// TODO: Support:
	// - UPDATED
	// - TOGETHER
	// - IGNORE
	// - QUIETLY
	// - PIECEMEAL
	// - EXISTING
	// - MAX_LINE_FACTOR

	while (remainder < end) {
		// transfer whitespace unchanged
		if (isspace(*remainder)) {
			commandLine += *remainder++;
			continue;
		}

		// get the next contiguous non-whitespace sequence
		const char* wordStart = remainder;
		bool needsExpansion = false;
		while (remainder < end && !isspace(*remainder)) {
			if (*remainder == '$')
				needsExpansion |= remainder + 1 < end && remainder[1] == '(';
			remainder++;
		}

		// append the sequence, expanding variables, if necessary
		if (needsExpansion) {
			StringList result = code::Leaf::EvaluateString(
				fEvaluationContext,
				wordStart,
				remainder,
				nullptr
			);
			bool isFirst = true;
			for (StringList::Iterator it = result.GetIterator();
				 it.HasNext();) {
				if (isFirst)
					isFirst = false;
				else
					commandLine += ' ';
				commandLine += it.Next();
			}
		} else
			commandLine.Append(wordStart, remainder - wordStart);
	}

	// reinstate the old local variable scope and the built-in variables
	fEvaluationContext.SetLocalScope(oldLocalScope);
	fEvaluationContext.SetBuiltInVariables(oldBuiltInVariables);

	return new Command(actionsCall, commandLine, boundTargets);
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

} // namespace ham::make
