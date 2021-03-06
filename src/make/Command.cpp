/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Copyright 2022, Dominic Martinez, dom@dominicm.dev.
 * Distributed under the terms of the MIT License.
 */

#include "make/Command.hpp"

#include "data/RuleActions.hpp"

namespace ham::make
{

Command::Command(
	data::RuleActionsCall* actions,
	const String commandLine,
	const StringList boundTargetPaths
)
	: fActions(actions),
	  fCommandLine(commandLine),
	  fBoundTargetPaths(boundTargetPaths),
	  fState(NOT_EXECUTED),
	  fWaitingBuildInfos()
{
	fActions->AcquireReference();
}

Command::~Command() { fActions->ReleaseReference(); }

} // namespace ham::make
