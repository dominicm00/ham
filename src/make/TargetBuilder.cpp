/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "make/TargetBuilder.hpp"

#include "data/RuleActions.hpp"
#include "make/Command.hpp"
#include "make/Options.hpp"
#include "make/TargetBuildInfo.hpp"
#include "process/ChildInfo.hpp"

namespace ham
{
namespace make
{

struct TargetBuilder::JobSlot {
	process::Process fProcess;
	Command* fCommand;

  public:
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

TargetBuilder::~TargetBuilder()
{
	delete[] fJobSlots;
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
				switch (command->GetState()) {
					case Command::NOT_EXECUTED:
					case Command::IN_PROGRESS:
						// cannot happen
						break;
					case Command::SUCCEEDED:
						_ExecuteNextCommand(buildInfo);
						break;
					case Command::FAILED:
						// TODO: Insert at head to allow for early error
						// detection?
						fFinishedBuildInfos.push_back(buildInfo);
						buildInfo->SetFailed(true);
						fBuildInfos.erase(std::find(fBuildInfos.begin(),
													fBuildInfos.end(),
													buildInfo));

						// print diagnostics
						printf("%s\n", command->CommandLine().ToCString());
						printf("...failed %s %s\n",
							   command->Actions()
								   ->Actions()
								   ->RuleName()
								   .ToCString(),
							   command->BoundTargetPaths()
								   .Join(StringPart(" "))
								   .ToCString());

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
		command->SetState(processInfo.fExitCode == 0 ? Command::SUCCEEDED
													 : Command::FAILED);
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
				std::find(fBuildInfos.begin(), fBuildInfos.end(), buildInfo));
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
				fBuildInfos.erase(std::find(fBuildInfos.begin(),
											fBuildInfos.end(),
											buildInfo));
				return;
		}
	}
}

void
TargetBuilder::_ExecuteCommand(Command* command)
{
	if (fOptions.IsPrintActions()) {
		data::RuleActionsCall* actions = command->Actions();
		printf("%s %s\n",
			   actions->Actions()->RuleName().ToCString(),
			   command->BoundTargetPaths().Join(StringPart(" ")).ToCString());
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
	//	if (jobSlot < 0)
	//		throw InternalError();
	char slotString[16];
	snprintf(slotString, sizeof(slotString), "%d", jobSlot + 1);

	// prepare the command line argument vector
	size_t argumentCount = fJamShell.Size() + 1;
	const char** arguments = new const char*[argumentCount + 1];
	for (size_t i = 0; i < argumentCount; i++) {
		String argument = fJamShell.ElementAt(i);
		if (argument == "%")
			arguments[i] = command->CommandLine().ToCString();
		else if (argument == "!")
			arguments[i] = slotString;
		else
			arguments[i] = argument.ToCString();
	}
	arguments[argumentCount] = nullptr;

	// launch the command
	bool launched = fJobSlots[jobSlot].fProcess.Launch(arguments[0],
													   arguments,
													   argumentCount);

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

} // namespace make
} // namespace ham
