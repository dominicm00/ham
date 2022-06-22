/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_MAKE_TARGET_BUILD_INFO_HPP
#define HAM_MAKE_TARGET_BUILD_INFO_HPP

#include "data/StringList.hpp"

namespace ham::make
{

class Command;
class MakeTarget;

class TargetBuildInfo
{
  public:
	TargetBuildInfo(MakeTarget* target);
	~TargetBuildInfo();

	MakeTarget* GetTarget() const { return fTarget; }

	const std::vector<Command*>& Commands() const { return fCommands; }
	void AddCommand(Command* command);
	Command* NextCommand();

	bool HasFailed() const { return fFailed; }

	void SetFailed(bool failed) { fFailed = failed; }

  private:
	MakeTarget* fTarget;
	std::vector<Command*> fCommands;
	size_t fCommandIndex;
	bool fFailed;
};

} // namespace ham::make

#endif // HAM_MAKE_TARGET_BUILD_INFO_HPP
