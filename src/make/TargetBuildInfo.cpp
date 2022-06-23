/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "make/TargetBuildInfo.hpp"

#include "make/Command.hpp"

namespace ham::make
{

class Command;
class MakeTarget;

TargetBuildInfo::TargetBuildInfo(MakeTarget* target)
	: fTarget(target),
	  fCommands(),
	  fCommandIndex(0),
	  fFailed(false)
{
}

TargetBuildInfo::~TargetBuildInfo()
{
	for (std::vector<Command*>::iterator it = fCommands.begin();
		 it != fCommands.end();
		 ++it) {
		(*it)->ReleaseReference();
	}
}

void
TargetBuildInfo::AddCommand(Command* command)
{
	fCommands.push_back(command);
	command->AcquireReference();
}

Command*
TargetBuildInfo::NextCommand()
{
	if (fCommandIndex >= fCommands.size())
		return nullptr;
	return fCommands[fCommandIndex++];
}

} // namespace ham::make
