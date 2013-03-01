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
	fPrintMakeTree(false),
	fPrintActions(false),
	fPrintCommands(false),
	fPrimaryTargetNames(),
	fMakeTargets(),
	fMakeLevel(0)
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
Processor::SetPrintMakeTree(bool printMakeTree)
{
	fPrintMakeTree = printMakeTree;
}


void
Processor::SetPrintActions(bool printActions)
{
	fPrintActions = printActions;
}


void
Processor::SetPrintCommands(bool printCommands)
{
	fPrintCommands = printCommands;
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
	fMakeLevel = 0;

	for (size_t i = 0; i < primaryTargetCount; i++) {
		String targetName = fPrimaryTargetNames.ElementAt(i);
		Target* target = fTargets.Lookup(targetName);
		_PrepareTargetRecursively(_GetMakeTarget(target, false), Time(0));
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

	if (fPrintMakeTree)
		_PrintMakeTreeStep(makeTarget, "make", NULL, NULL);

	makeTarget->SetFate(MakeTarget::PROCESSING);

	// bind the target
	Target* target = makeTarget->GetTarget();
	bool isPseudoTarget = target->IsNotAFile();
	if (isPseudoTarget)
		makeTarget->SetTime(Time(0));
	else
		data::TargetBinder::Bind(fGlobalVariables, makeTarget);

	Time time = makeTarget->GetTime();
	if (!time.IsValid())
		time = Time(0);

	if (fPrintMakeTree)
		_PrintMakeTreeBinding(makeTarget);

	// add make targets for dependencies
	const TargetSet& dependencies = target->Dependencies();
	for (TargetSet::iterator it = dependencies.begin();
		it != dependencies.end(); ++it) {
		makeTarget->AddDependency(_GetMakeTarget(*it, true));
	}

	// recursively process the target's dependencies
	Time newestDependencyTime(0);
	Time newestLeafTime(0);
	bool dependencyUpdated = false;
	bool cantMake = false;
	for (size_t i = 0; i < makeTarget->Dependencies().size(); i++) {
		MakeTarget* dependency = makeTarget->Dependencies().at(i);

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
	for (TargetSet::iterator it = includes.begin();
		it != includes.end(); ++it) {
		makeTarget->AddInclude(_GetMakeTarget(*it, true));
	}

	// For depends-on-leaves targets consider only the leaf times.
	if (target->DependsOnLeaves())
		newestDependencyTime = newestLeafTime;

	// determine the target's state
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

	if (fPrintMakeTree)
		_PrintMakeTreeState(makeTarget, parentTime);
// TODO: Support:
// - BUILD_ALWAYS
// - IGNORE_IF_MISSING
// - TEMPORARY
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
		// influence of the target.
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
