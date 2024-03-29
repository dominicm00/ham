/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Copyright 2022, Dominic Martinez, dom@dominicm.dev.
 * Distributed under the terms of the MIT License.
 */

#include "make/TargetBuilder.hpp"

#include "data/RuleActions.hpp"
#include "make/Command.hpp"
#include "make/Options.hpp"
#include "make/TargetBuildInfo.hpp"
#include "process/ChildInfo.hpp"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>

namespace ham::make
{

class TargetBuilder::JobSlot
{
  public:
	process::Process fProcess;
	Command* fCommand;

	JobSlot()
		: fProcess(),
		  fCommand(nullptr)
	{
	}
};

TargetBuilder::TargetBuilder(const Options& options, const StringList& jamShell)
	: fOptions(options),
	  fMaxJobCount(options.JobCount()),
	  fJamShell(jamShell),
	  fBuildInfos(),
	  fFinishedBuildInfos(),
	  fFinishedCommands(),
	  fJobSlots(new JobSlot[fMaxJobCount])
{
}

TargetBuilder::~TargetBuilder() { delete[] fJobSlots; }

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
TargetBuilder::NextFinishedBuildInfo(bool canWait)
{
	for (;;) {
		// handle finished commands
		while (!fFinishedCommands.empty()) {
			Command* command = fFinishedCommands.front();
			fFinishedCommands.erase(fFinishedCommands.begin());
			for (std::vector<TargetBuildInfo*>::const_iterator it =
					 command->WaitingBuildInfos().begin();
				 it != command->WaitingBuildInfos().end();
				 ++it) {
				TargetBuildInfo* buildInfo = *it;

				auto state = command->GetState();
				if (command->Actions()->Actions()->IsIgnore()
					&& state == Command::FAILED)
					state = Command::SUCCEEDED;

				switch (state) {
					case Command::NOT_EXECUTED:
					case Command::IN_PROGRESS:
						// cannot happen
						// TODO: exception?
						break;
					case Command::SUCCEEDED:
						_ExecuteNextCommand(buildInfo);
						break;
					case Command::FAILED:
						// TODO: Insert at head to allow for early error
						// detection?
						fFinishedBuildInfos.push_back(buildInfo);
						buildInfo->SetFailed(true);
						fBuildInfos.erase(std::find(
							fBuildInfos.begin(),
							fBuildInfos.end(),
							buildInfo
						));

						// print diagnostics
						printf("%s\n", command->CommandLine().ToCString());
						printf(
							"...failed %s %s\n",
							command->Actions()->Actions()->RuleName().ToCString(
							),
							command->BoundTargetPaths()
								.Join(StringPart(" "))
								.ToCString()
						);

						// remove the targets
						for (StringList::Iterator it =
								 command->BoundTargetPaths().GetIterator();
							 it.HasNext();) {
							String path = it.Next();
							// TODO: Platform specific!
							if (unlink(path.ToCString()) == 0)
								printf("...removing %s\n", path.ToCString());
						}
						break;
				}
			}

			command->ClearWaitingBuildInfos();
		}

		if (!fFinishedBuildInfos.empty()) {
			TargetBuildInfo* buildInfo = fFinishedBuildInfos.front();
			fFinishedBuildInfos.erase(fFinishedBuildInfos.begin());
			return buildInfo;
		}

		if (!canWait || fBuildInfos.empty())
			return nullptr;

		// wait for some running command to finish
		process::ChildInfo processInfo;
		if (!process::Process::WaitForChild(processInfo)
			|| !processInfo.fExited) {
			continue;
		}

		int jobSlot = _FindJobSlot(processInfo.fId);
		if (jobSlot < 0)
			continue;

		Command* command = fJobSlots[jobSlot].fCommand;
		fJobSlots[jobSlot].fCommand = nullptr;
		fJobSlots[jobSlot].fProcess.Unset();

		fFinishedCommands.push_back(command);
		Command::State state = processInfo.fExitCode == 0
				|| command->Actions()->Actions()->IsIgnore()
			? Command::SUCCEEDED
			: Command::FAILED;
		command->SetState(state);

		// TODO: This lets new commands enter the queue before quitting. Make
		// this quit immediately.
		if (state == Command::FAILED && fOptions.IsQuitOnError()) {
			int exitCode = processInfo.fExitCode;

			printf("...failed to execute command, exiting...\n");
			printf("%s\n", command->CommandLine().ToCString());
			printf("...waiting for commands to exit...\n");
			// Wait for children
			while (process::Process::WaitForChild(processInfo))
				;
			printf("...children done, exiting...\n");

			exit(exitCode);
		}
	}
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
		if (command == nullptr) {
			fFinishedBuildInfos.push_back(buildInfo);
			fBuildInfos.erase(
				std::find(fBuildInfos.begin(), fBuildInfos.end(), buildInfo)
			);
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
					std::find(fBuildInfos.begin(), fBuildInfos.end(), buildInfo)
				);
				return;
		}
	}
}

void
TargetBuilder::_ExecuteCommand(Command* command)
{
	if (fOptions.IsPrintActions()) {
		data::RuleActionsCall* actions = command->Actions();

		if (fOptions.IsPrintQuietActions()
			|| !(actions->Actions()->IsQuietly())) {
			printf(
				"%s %s\n",
				actions->Actions()->RuleName().ToCString(),
				command->BoundTargetPaths().Join(StringPart(" ")).ToCString()
			);
		}
	}

	if (fOptions.IsPrintCommands()) {
		printf("%s\n", command->CommandLine().ToCString());
	}

	if (fOptions.IsDryRun()) {
		command->SetState(Command::SUCCEEDED);
		fFinishedCommands.push_back(command);
		return;
	}

	int jobSlot = _FindFreeJobSlot();
	// TODO:...
	if (jobSlot < 0) {
		throw std::logic_error("Could not find job slot for command");
	}
	char slotString[16];
	snprintf(slotString, sizeof(slotString), "%d", jobSlot + 1);

	// prepare the command line argument vector
	size_t argumentCount = fJamShell.Size();
	const char** arguments = new const char*[argumentCount + 2];
	bool addedCommand = false;
	for (size_t i = 0; i < argumentCount; i++) {
		String argument = fJamShell.ElementAt(i);
		if (argument == "%") {
			arguments[i] = command->CommandLine().ToCString();
			addedCommand = true;
		} else if (argument == "!") {
			arguments[i] = slotString;
		} else {
			arguments[i] = argument.ToCString();
		}
	}

	if (!addedCommand) {
		// Append command to end if not explicitly added
		arguments[argumentCount] = command->CommandLine().ToCString();
		arguments[argumentCount + 1] = nullptr;
	} else {
		arguments[argumentCount] = nullptr;
	}

	// launch the command
	bool launched = fJobSlots[jobSlot].fProcess.Launch(
		arguments[0],
		arguments,
		argumentCount
	);

	delete[] arguments;

	if (!launched) {
		command->SetState(Command::FAILED);
		fFinishedCommands.push_back(command);
		return;
	}

	command->SetState(Command::IN_PROGRESS);
	fJobSlots[jobSlot].fCommand = command;
}

int
TargetBuilder::_FindFreeJobSlot() const
{
	for (size_t i = 0; i < fMaxJobCount; i++) {
		if (fJobSlots[i].fCommand == nullptr)
			return (int)i;
	}

	return -1;
}

int
TargetBuilder::_FindJobSlot(process::Process::Id id) const
{
	for (size_t i = 0; i < fMaxJobCount; i++) {
		if (fJobSlots[i].fProcess.IsValid()
			&& fJobSlots[i].fProcess.GetId() == id) {
			return (int)i;
		}
	}

	return -1;
}

} // namespace ham::make
