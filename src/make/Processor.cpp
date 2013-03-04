/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "make/Processor.h"

#include <stdarg.h>
#include <fstream>
#include <memory>

#include "code/Block.h"
#include "code/BuiltInRules.h"
#include "code/Constant.h"
#include "code/FunctionCall.h"
#include "code/Jambase.h"
#include "code/Leaf.h"
#include "code/OnExpression.h"
#include "data/RegExp.h"
#include "data/TargetBinder.h"
#include "make/MakeException.h"
#include "parser/Parser.h"


namespace ham {
namespace make {


using data::Time;


static const String kHeaderScanVariableName("HDRSCAN");
static const String kHeaderRuleVariableName("HDRRULE");


Processor::DebugOptions::DebugOptions()
	:
	fDryRun(false),
	fPrintMakeTree(false),
	fPrintActions(false),
	fPrintCommands(false)
{
}


struct Processor::Command : util::Referenceable {
	enum State {
		NOT_EXECUTED,
		IN_PROGRESS,
		SUCCEEDED,
		FAILED
	};

public:
	Command(data::RuleActionsCall* actions, const String& commandLine,
		const StringList& boundTargetPaths)
		:
		fActions(actions),
		fCommandLine(commandLine),
		fBoundTargetPaths(boundTargetPaths),
		fState(NOT_EXECUTED),
		fWaitingBuildInfos()
	{
		fActions->AcquireReference();
	}

	~Command()
	{
		fActions->ReleaseReference();
	}

	data::RuleActionsCall* Actions() const
	{
		return fActions;
	}

	const String& CommandLine() const
	{
		return fCommandLine;
	}

	const StringList& BoundTargetPaths() const
	{
		return fBoundTargetPaths;
	}

	State GetState() const
	{
		return fState;
	}

	void SetState(State state)
	{
		fState = state;
	}

	const std::vector<TargetBuildInfo*>& WaitingBuildInfos() const
	{
		return fWaitingBuildInfos;
	}

	void AddWaitingBuildInfo(TargetBuildInfo* buildInfo)
	{
		fWaitingBuildInfos.push_back(buildInfo);
	}

	void ClearWaitingBuildInfos()
	{
		fWaitingBuildInfos.clear();
	}

private:
	data::RuleActionsCall*			fActions;
	String							fCommandLine;
	StringList						fBoundTargetPaths;
	State							fState;
	std::vector<TargetBuildInfo*>	fWaitingBuildInfos;
};


struct Processor::TargetBuildInfo {
	TargetBuildInfo(MakeTarget* target)
		:
		fTarget(target),
		fCommands(),
		fCommandIndex(0),
		fFailed(false)
	{
	}

	~TargetBuildInfo()
	{
		for (std::vector<Command*>::iterator it = fCommands.begin();
			it != fCommands.end(); ++it) {
			(*it)->ReleaseReference();
		}
	}

	MakeTarget* GetTarget() const
	{
		return fTarget;
	}

	const std::vector<Command*>& Commands() const
	{
		return fCommands;
	}

	void AddCommand(Command* command)
	{
		fCommands.push_back(command);
		command->AcquireReference();
	}

	Command* NextCommand()
	{
		if (fCommandIndex >= fCommands.size())
			return NULL;
		return fCommands[fCommandIndex++];
	}

	bool HasFailed() const
	{
		return fFailed;
	}

	void SetFailed(bool failed)
	{
		fFailed = failed;
	}

private:
	MakeTarget*				fTarget;
	std::vector<Command*>	fCommands;
	size_t					fCommandIndex;
	bool					fFailed;
};


struct Processor::TargetBuilder {
	TargetBuilder(const DebugOptions& debugOptions, size_t maxJobCount)
		:
		fDebugOptions(debugOptions),
		fMaxJobCount(maxJobCount),
		fBuildInfos()
	{
	}

	bool HasSpareJobSlots() const
	{
		return fMaxJobCount > fBuildInfos.size();
	}

	void AddBuildInfo(TargetBuildInfo* buildInfo)
	{
		fBuildInfos.push_back(buildInfo);
// TODO: Ownership?
		_ExecuteNextCommand(buildInfo);
	}

	TargetBuildInfo* NextFinishedBuildInfo()
	{
		// handle finished commands
		while (!fFinishedCommands.empty()) {
			Command* command = fFinishedCommands.front();
			fFinishedCommands.erase(fFinishedCommands.begin());
			for (std::vector<TargetBuildInfo*>::const_iterator it
					= command->WaitingBuildInfos().begin();
				it != command->WaitingBuildInfos().end(); ++it) {
				TargetBuildInfo* buildInfo = *it;
				switch (command->GetState()) {
					case Command::NOT_EXECUTED:
					case Command::IN_PROGRESS:
						// cannot happen
						break;
					case Command::SUCCEEDED:
						_ExecuteNextCommand(buildInfo);
						break;
					case Command::FAILED:
// TODO: Insert at head to allow for early error detection?
						fFinishedBuildInfos.push_back(buildInfo);
						buildInfo->SetFailed(true);
						fBuildInfos.erase(
							std::find(fBuildInfos.begin(), fBuildInfos.end(),
								buildInfo));
						break;
				}
			}

			command->ClearWaitingBuildInfos();
		}

		if (fFinishedBuildInfos.empty())
			return NULL;

		TargetBuildInfo* buildInfo = fFinishedBuildInfos.front();
		fFinishedBuildInfos.erase(fFinishedBuildInfos.begin());
		return buildInfo;
	}

	bool HasPendingBuildInfos() const
	{
		return !fBuildInfos.empty() || !fFinishedBuildInfos.empty();
	}

private:
	void _ExecuteNextCommand(TargetBuildInfo* buildInfo)
	{
		for (;;) {
			Command* command = buildInfo->NextCommand();
			if (command == NULL) {
				fFinishedBuildInfos.push_back(buildInfo);
				fBuildInfos.erase(
					std::find(fBuildInfos.begin(), fBuildInfos.end(),
					buildInfo));
				return;
			}

			switch (command->GetState()) {
				case Command::NOT_EXECUTED:
					// execute command
					command->AddWaitingBuildInfo(buildInfo);
					_ExecuteCommand(command);
					return;
				case Command::IN_PROGRESS:
					// wait for command to finish
					command->AddWaitingBuildInfo(buildInfo);
					return;
				case Command::SUCCEEDED:
					// next command...
					break;
				case Command::FAILED:
// TODO: Insert at head to allow for early error detection?
					fFinishedBuildInfos.push_back(buildInfo);
					buildInfo->SetFailed(true);
					fBuildInfos.erase(
						std::find(fBuildInfos.begin(), fBuildInfos.end(),
							buildInfo));
					return;
			}
		}
	}

	void _ExecuteCommand(Command* command)
	{
		if (fDebugOptions.fPrintActions) {
			data::RuleActionsCall* actions = command->Actions();
			printf("%s %s\n", actions->Actions()->RuleName().ToCString(),
				command->BoundTargetPaths().Join(StringPart(" ")).ToCString());
		}

		if (fDebugOptions.fPrintCommands) {
			printf("%s\n", command->CommandLine().ToCString());
		}

		if (fDebugOptions.fDryRun) {
			command->SetState(Command::SUCCEEDED);
			fFinishedCommands.push_back(command);
			return;
		}

// TODO:...
	}

private:
	const DebugOptions&				fDebugOptions;
	size_t							fMaxJobCount;
	std::vector<TargetBuildInfo*>	fBuildInfos;
	std::vector<TargetBuildInfo*>	fFinishedBuildInfos;
	std::vector<Command*>			fFinishedCommands;
};


Processor::Processor()
	:
	fGlobalVariables(),
	fTargets(),
	fEvaluationContext(fGlobalVariables, fTargets),
	fJambaseFile(),
	fActionsOutputFile(),
	fJobCount(1),
	fBuildFromNewest(false),
	fQuitOnError(false),
	fDebugOptions(),
	fPrimaryTargetNames(),
	fPrimaryTargets(),
	fMakeTargets(),
	fMakeLevel(0),
	fMakableTargets(),
	fCommands(),
	fTargetBuilders()
{
	code::BuiltInRules::RegisterRules(fEvaluationContext.Rules());
}


Processor::~Processor()
{
	for (TargetBuildInfoSet::iterator it = fTargetBuilders.begin();
		it != fTargetBuilders.end(); ++it) {
		delete *it;
	}

	for (CommandMap::iterator it = fCommands.begin(); it != fCommands.end();
		++it) {
		delete it->second;
	}

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
	fDebugOptions.fDryRun = dryRun;
}


void
Processor::SetQuitOnError(bool quitOnError)
{
	fQuitOnError = quitOnError;
}


void
Processor::SetPrintMakeTree(bool printMakeTree)
{
	fDebugOptions.fPrintMakeTree = printMakeTree;
}


void
Processor::SetPrintActions(bool printActions)
{
	fDebugOptions.fPrintActions = printActions;
}


void
Processor::SetPrintCommands(bool printCommands)
{
	fDebugOptions.fPrintCommands = printCommands;
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
// TODO: Not used yet!

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
	fMakeLevel = 0;

	for (size_t i = 0; i < primaryTargetCount; i++) {
		String targetName = fPrimaryTargetNames.ElementAt(i);
		Target* target = fTargets.Lookup(targetName);
		MakeTarget* makeTarget = _GetMakeTarget(target, false);
		fPrimaryTargets.Append(makeTarget);
		_PrepareTargetRecursively(makeTarget, Time(0));
	}
}


void
Processor::BuildTargets()
{
	for (MakeTargetSet::Iterator it = fPrimaryTargets.GetIterator();
		it.HasNext();) {
		_CollectMakableTargets(it.Next());
	}

	TargetBuilder builder(fDebugOptions, fJobCount);

	while (!fMakableTargets.IsEmpty() || builder.HasPendingBuildInfos()) {
		while (TargetBuildInfo* buildInfo = builder.NextFinishedBuildInfo()) {
			_TargetMade(buildInfo->GetTarget(),
				buildInfo->HasFailed() ? MakeTarget::FAILED : MakeTarget::DONE);
		}

		while (builder.HasSpareJobSlots() && !fMakableTargets.IsEmpty()) {
			MakeTarget* makeTarget = fMakableTargets.Head();
			fMakableTargets.RemoveAt(0);
			builder.AddBuildInfo(_MakeTarget(makeTarget));
		}
	}
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


MakeTarget*
Processor::_GetMakeTarget(const String& targetName, bool create)
{
	Target* target = create
		? fTargets.LookupOrCreate(targetName) : fTargets.Lookup(targetName);
	return target != NULL ? _GetMakeTarget(target, create) : NULL;
}


void
Processor::_PrepareTargetRecursively(MakeTarget* makeTarget,
	data::Time parentTime)
{
	// Check whether the target has already been processed (also detect cycles)
	// and mark in-progress.
	if (makeTarget->GetFate() != MakeTarget::UNPROCESSED) {
		if (makeTarget->GetFate() == MakeTarget::PROCESSING) {
			throw MakeException(std::string("Target \"")
				+ makeTarget->Name().ToCString() + "\" depends on itself");
		}

		// already done
		return;
	}

	if (fDebugOptions.fPrintMakeTree)
		_PrintMakeTreeStep(makeTarget, "make", NULL, NULL);

	makeTarget->SetFate(MakeTarget::PROCESSING);

	// bind the target
	_BindTarget(makeTarget);

	Target* target = makeTarget->GetTarget();
	bool isPseudoTarget = target->IsNotAFile();
	if (isPseudoTarget)
		makeTarget->SetTime(Time(0));

	Time time = makeTarget->GetTime();
	if (!time.IsValid())
		time = Time(0);

	if (fDebugOptions.fPrintMakeTree)
		_PrintMakeTreeBinding(makeTarget);

	// add make targets for dependencies
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
		_PrepareTargetRecursively(dependency, time);
		fMakeLevel--;

		// Add the dependency's includes as the target's dependencies. This
		// will also take care of recursive includes in a breadth first manner
		// as we keep appending the newly found includes at the end of our
		// dependency list.
		makeTarget->AddDependencies(dependency->Includes());

		// track times
		newestDependencyTime = std::max(newestDependencyTime,
			dependency->GetTime());
		newestLeafTime = std::max(newestLeafTime, dependency->LeafTime());

		switch (dependency->GetFate()) {
			case MakeTarget::UNPROCESSED:
			case MakeTarget::PROCESSING:
				// cannot happen
				break;
			case MakeTarget::KEEP:
				break;
			case MakeTarget::MAKE:
				dependencyUpdated = true;
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
	if (target->IsDontUpdate())
		time = Time(0);

	// determine the target's state and fate
	MakeTarget::State state;
	MakeTarget::Fate fate = MakeTarget::KEEP;
	if (isPseudoTarget || !makeTarget->FileExists()) {
		state = MakeTarget::MISSING;
		if (cantMake)
			fate = MakeTarget::CANT_MAKE;
		else
			fate = MakeTarget::MAKE;
	} else if (newestDependencyTime > time) {
		state = MakeTarget::OUT_OF_DATE;
		if (cantMake)
			fate = MakeTarget::CANT_MAKE;
		else if (!target->IsDontUpdate())
			fate = MakeTarget::MAKE;
	} else {
		state = MakeTarget::UP_TO_DATE;
		if (dependencyUpdated)
			fate = MakeTarget::MAKE;
	}

	if (fate == MakeTarget::MAKE && !target->HasActionsCalls()) {
		if (!isPseudoTarget) {
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

	if (fDebugOptions.fPrintMakeTree)
		_PrintMakeTreeState(makeTarget, parentTime);
// TODO: Support:
// - BUILD_ALWAYS
// - IGNORE_IF_MISSING
// - TEMPORARY
}


void
Processor::_BindTarget(MakeTarget* makeTarget)
{
	if (makeTarget->IsBound())
		return;

	Target* target = makeTarget->GetTarget();

	String boundPath;
	data::FileStatus fileStatus;
	data::TargetBinder::Bind(*fEvaluationContext.GlobalVariables(), target,
		boundPath, fileStatus);
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
	if (variables == NULL)
		return;

	const StringList* scanPattern = variables->Lookup(kHeaderScanVariableName);
	const StringList* scanRule = variables->Lookup(kHeaderRuleVariableName);
	if (scanPattern == NULL || scanRule == NULL || scanPattern->IsEmpty()
		|| scanRule->IsEmpty()) {
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
				String headerName(line.c_str() + startOffset,
					endOffset - startOffset);
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
		code::NodeReference targetNameNode(new code::Constant(target->Name()),
			true);
		code::NodeReference headersNode(new code::Constant(headersFound), true);
		code::NodeReference callFunction(new code::Constant(scanRule), true);
		util::Reference<code::FunctionCall> call(
			new code::FunctionCall(callFunction.Get()), true);
		call->AddArgument(targetNameNode.Get());
		call->AddArgument(headersNode.Get());
		code::NodeReference onExpression(
			new code::OnExpression(targetNameNode.Get(), call.Get()), true);
		onExpression->Evaluate(fEvaluationContext);
	}
}


bool
Processor::_CollectMakableTargets(MakeTarget* makeTarget)
{
	switch (makeTarget->GetFate()) {
		case MakeTarget::MAKE:
			makeTarget->SetMakeState(MakeTarget::PENDING);
			break;
		case MakeTarget::KEEP:
			makeTarget->SetMakeState(MakeTarget::DONE);
			return false;
		case MakeTarget::UNPROCESSED:
		case MakeTarget::PROCESSING:
			// those can't happen
		case MakeTarget::CANT_MAKE:
			makeTarget->SetMakeState(MakeTarget::SKIPPED);
			return false;
	}

	size_t pendingDependencyCount = 0;
	for (MakeTargetSet::Iterator it = makeTarget->Dependencies().GetIterator();
		it.HasNext();) {
		if (_CollectMakableTargets(it.Next()))
			pendingDependencyCount++;
	}

	makeTarget->SetPendingDependenciesCount(pendingDependencyCount);

	if (pendingDependencyCount == 0)
		fMakableTargets.Append(makeTarget);

	return true;
}


Processor::TargetBuildInfo*
Processor::_MakeTarget(MakeTarget* makeTarget)
{
	Target* target = makeTarget->GetTarget();
	if (target->ActionsCalls().empty()) {
		_TargetMade(makeTarget, MakeTarget::DONE);
		return NULL;
	}

	std::auto_ptr<TargetBuildInfo> buildInfo(new TargetBuildInfo(makeTarget));

	for (std::vector<data::RuleActionsCall*>::const_iterator it
			= target->ActionsCalls().begin();
		it != target->ActionsCalls().end(); ++it) {
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


void
Processor::_TargetMade(MakeTarget* makeTarget, MakeTarget::MakeState state)
{
	if (makeTarget->GetMakeState() == MakeTarget::PENDING)
		makeTarget->SetMakeState(state);

	switch (state) {
		case MakeTarget::DONE:
			break;
		case MakeTarget::PENDING:
			// cannot happen
			break;
		case MakeTarget::FAILED:
//TODO: Reporting should happen where we know what action failed.
//			printf("");
			break;
		case MakeTarget::SKIPPED:
		{
			// get the first dependency that couldn't be made
			MakeTarget* lackingDependency = NULL;
			for (MakeTargetSet::Iterator it
					= makeTarget->Dependencies().GetIterator();
				it.HasNext();) {
				MakeTarget* dependency = it.Next();
				if (dependency->GetMakeState() == MakeTarget::FAILED
					|| dependency->GetMakeState() == MakeTarget::SKIPPED) {
					lackingDependency = dependency;
				}
			}

			printf("...skipped %s for lack of %s...\n",
				makeTarget->Name().ToCString(),
				lackingDependency != NULL
					? lackingDependency->Name().ToCString() : "???");
			break;
		}
	}

	// propagate the event to the target's parents
	for (MakeTargetSet::Iterator it = makeTarget->Parents().GetIterator();
		it.HasNext();) {
		MakeTarget* parent = it.Next();
		size_t pendingDependencyCount = parent->PendingDependenciesCount() - 1;
		parent->SetPendingDependenciesCount(pendingDependencyCount);

		if (pendingDependencyCount == 0) {
			if (parent->GetMakeState() == MakeTarget::PENDING)
				fMakableTargets.Insert( parent, 0);
			else
				_TargetMade(parent, MakeTarget::SKIPPED);
		}
	}
}


Processor::Command*
Processor::_BuildCommand(data::RuleActionsCall* actionsCall)
{
	// create a variable domain for the built-in variables (the numbered ones
	// and "<" and ">")
	data::VariableDomain builtInVariables;

	StringList boundTargets;
	for (data::TargetList::const_iterator it = actionsCall->Targets().begin();
		it != actionsCall->Targets().end(); ++it) {
		MakeTarget* makeTarget = _GetMakeTarget(*it, true);
		boundTargets.Append(makeTarget->BoundPath());
	}
	builtInVariables.Set("1", boundTargets);
	builtInVariables.Set("<", boundTargets);

	StringList boundSourceTargets;
	for (data::TargetList::const_iterator it
			= actionsCall->SourceTargets().begin();
		it != actionsCall->SourceTargets().end(); ++it) {
		MakeTarget* makeTarget = _GetMakeTarget(*it, true);
		boundSourceTargets.Append(makeTarget->BoundPath());
	}
	builtInVariables.Set("2", boundSourceTargets);
	builtInVariables.Set(">", boundSourceTargets);

	// get the first of the targets and push a copy of its variable domain as a
	// new local scope
	data::Target* target = *actionsCall->Targets().begin();
	data::VariableDomain localVariables(*target->Variables(true));
	data::VariableScope* oldLocalScope = fEvaluationContext.LocalScope();
	data::VariableScope localScope(localVariables, oldLocalScope);

//	// prepare the local variable scope (for the named parameters)
//	data::VariableDomain localVariables;
//	data::VariableScope localScope(localVariables, oldLocalScope);
//		// TODO: This is jam compatible behavior. It would be more logical to
//		// have a NULL parent for the new scope, so the previous local variables
//		// cannot be seen in the rule block.

//	// set the named parameters
//	StringList::Iterator it = fParameterNames.GetIterator();
//	for (size_t i = 0; i < parameterCount && it.HasNext(); i++)
//		localVariables.Set(it.Next(), parameters[i]);

	// set the local variable scope and the built-in variables
	fEvaluationContext.SetLocalScope(&localScope);

	data::VariableDomain* oldBuiltInVariables
		= fEvaluationContext.BuiltInVariables();
	fEvaluationContext.SetBuiltInVariables(&builtInVariables);

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
// TODO: Set source and target as well as explicitly bound variables!
			StringList result = code::Leaf::EvaluateString(fEvaluationContext,
				wordStart, remainder, NULL);
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
	const Target* target = makeTarget->GetTarget();
	const char* timeString;
	if (makeTarget->FileExists()) {
		_PrintMakeTreeStep(makeTarget, "bind", NULL, ": %s",
			makeTarget->BoundPath().ToCString());
		timeString = makeTarget->GetTime().ToString().ToCString();
	} else {
		if (target->IsNotAFile())
			timeString = "unbound";
		else
			timeString = "missing";
	}

// TODO: Binding might also be "parent".
	_PrintMakeTreeStep(makeTarget, "time", NULL, ": %s", timeString);
}


void
Processor::_PrintMakeTreeState(const MakeTarget* makeTarget,
	data::Time parentTime)
{
	const Target* target = makeTarget->GetTarget();
	const char* stateString;
	char madeString[6] = "made ";
	char& flag = madeString[4];

	switch (makeTarget->GetFate()) {
		case MakeTarget::UNPROCESSED:
			stateString = "unprocessed";
			break;
		case MakeTarget::PROCESSING:
			stateString = "processing";
			break;
		case MakeTarget::MAKE:
			stateString = makeTarget->FileExists() || target->IsNotAFile()
				? "update" : "missing";
			if (!target->IsNotAFile())
				flag = '+';
			break;
		case MakeTarget::KEEP:
			if (makeTarget->GetTime() > parentTime) {
				flag = '*';
				stateString = "newer";
			} else
				stateString = "stable";
			break;
		case MakeTarget::CANT_MAKE:
			stateString = "missing";
			break;
		default:
			stateString = "unknown";
			break;
	}

	_PrintMakeTreeStep(makeTarget, madeString, stateString, NULL);
}


void
Processor::_PrintMakeTreeStep(const MakeTarget* makeTarget, const char* step,
	const char* state, const char* pattern, ...)
{
	printf("%-7s %-10s %*s%s", step, state != NULL ? state : "--", fMakeLevel,
		"", makeTarget->Name().ToCString());
	if (pattern != NULL) {
		char buffer[1024];
		va_list args;
		va_start(args, pattern);
		vsnprintf(buffer, sizeof(buffer), pattern, args);
		va_end(args);
		printf("%s", buffer);
	}

	printf("\n");
}


}	// namespace make
}	// namespace ham
