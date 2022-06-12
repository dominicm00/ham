/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_MAKE_COMMAND_H
#define HAM_MAKE_COMMAND_H

#include "data/StringList.h"
#include "util/Referenceable.h"

namespace ham
{

namespace data
{
class RuleActionsCall;
}

namespace make
{

class TargetBuildInfo;

struct Command : util::Referenceable {
	enum State { NOT_EXECUTED, IN_PROGRESS, SUCCEEDED, FAILED };

  public:
	Command(data::RuleActionsCall* actions,
			const String& commandLine,
			const StringList& boundTargetPaths);
	~Command();

	data::RuleActionsCall* Actions() const { return fActions; }

	const String& CommandLine() const { return fCommandLine; }

	const StringList& BoundTargetPaths() const { return fBoundTargetPaths; }

	State GetState() const { return fState; }
	void SetState(State state) { fState = state; }

	const std::vector<TargetBuildInfo*>& WaitingBuildInfos() const
	{
		return fWaitingBuildInfos;
	}
	void AddWaitingBuildInfo(TargetBuildInfo* buildInfo)
	{
		fWaitingBuildInfos.push_back(buildInfo);
	}
	void ClearWaitingBuildInfos() { fWaitingBuildInfos.clear(); }

  private:
	data::RuleActionsCall* fActions;
	String fCommandLine;
	StringList fBoundTargetPaths;
	State fState;
	std::vector<TargetBuildInfo*> fWaitingBuildInfos;
};

} // namespace make
} // namespace ham

#endif // HAM_MAKE_COMMAND_H
