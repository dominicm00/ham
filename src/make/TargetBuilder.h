/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_MAKE_TARGET_BUILDER_H
#define HAM_MAKE_TARGET_BUILDER_H

#include <stddef.h>

#include <vector>

#include "data/StringList.h"
#include "process/Process.h"

namespace ham
{
namespace make
{

class Command;
class Options;
class TargetBuildInfo;

class TargetBuilder
{
  public:
	TargetBuilder(const Options& options, const StringList& jamShell);
	~TargetBuilder();

	bool HasSpareJobSlots() const;

	void AddBuildInfo(TargetBuildInfo* buildInfo);

	TargetBuildInfo* NextFinishedBuildInfo(bool canWait);
	bool HasPendingBuildInfos() const;

  private:
	struct JobSlot;

  private:
	void _ExecuteNextCommand(TargetBuildInfo* buildInfo);
	void _ExecuteCommand(Command* command);
	int _FindFreeJobSlot() const;
	int _FindJobSlot(process::Process::Id id) const;

  private:
	const Options& fOptions;
	size_t fMaxJobCount;
	StringList fJamShell;
	std::vector<TargetBuildInfo*> fBuildInfos;
	std::vector<TargetBuildInfo*> fFinishedBuildInfos;
	std::vector<Command*> fFinishedCommands;
	JobSlot* fJobSlots;
};

} // namespace make
} // namespace ham

#endif // HAM_MAKE_TARGET_BUILDER_H
