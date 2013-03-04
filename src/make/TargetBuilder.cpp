/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "make/TargetBuilder.h"

#include "data/RuleActions.h"
#include "make/Command.h"
#include "make/DebugOptions.h"
#include "make/TargetBuildInfo.h"


namespace ham {
namespace make {


TargetBuilder::TargetBuilder(const DebugOptions& debugOptions,
	size_t maxJobCount)
	:
	fDebugOptions(debugOptions),
	fMaxJobCount(maxJobCount),
	fBuildInfos()
{
}


bool
TargetBuilder::HasSpareJobSlots() const
{
	return fMaxJobCount > fBuildInfos.size();
}


void
TargetBuilder::AddBuildInfo(TargetBuildInfo* buildInfo)
{
	fBuildInfos.push_back(buildInfo);
// TODO: Ownership?
	_ExecuteNextCommand(buildInfo);
}


TargetBuildInfo*
TargetBuilder::NextFinishedBuildInfo()
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


bool
TargetBuilder::HasPendingBuildInfos() const
{
	return !fBuildInfos.empty() || !fFinishedBuildInfos.empty();
}


void
TargetBuilder::_ExecuteNextCommand(TargetBuildInfo* buildInfo)
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


void
TargetBuilder::_ExecuteCommand(Command* command)
{
	if (fDebugOptions.IsPrintActions()) {
		data::RuleActionsCall* actions = command->Actions();
		printf("%s %s\n", actions->Actions()->RuleName().ToCString(),
			command->BoundTargetPaths().Join(StringPart(" ")).ToCString());
	}

	if (fDebugOptions.IsPrintCommands()) {
		printf("%s\n", command->CommandLine().ToCString());
	}

	if (fDebugOptions.IsDryRun()) {
		command->SetState(Command::SUCCEEDED);
		fFinishedCommands.push_back(command);
		return;
	}

// TODO:...
}


} // namespace make
} // namespace ham
