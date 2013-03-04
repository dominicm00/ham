/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_MAKE_TARGET_BUILDER_H
#define HAM_MAKE_TARGET_BUILDER_H


#include <stddef.h>

#include <vector>


namespace ham {
namespace make {


class Command;
class DebugOptions;
class TargetBuildInfo;


class TargetBuilder {
public:
								TargetBuilder(const DebugOptions& debugOptions,
									size_t maxJobCount);

			bool				HasSpareJobSlots() const;

			void				AddBuildInfo(TargetBuildInfo* buildInfo);

			TargetBuildInfo*	NextFinishedBuildInfo();
			bool				HasPendingBuildInfos() const;

private:
			void				_ExecuteNextCommand(TargetBuildInfo* buildInfo);
			void				_ExecuteCommand(Command* command);

private:
			const DebugOptions&	fDebugOptions;
			size_t				fMaxJobCount;
			std::vector<TargetBuildInfo*> fBuildInfos;
			std::vector<TargetBuildInfo*> fFinishedBuildInfos;
			std::vector<Command*> fFinishedCommands;
};


} // namespace make
} // namespace ham


#endif // HAM_MAKE_TARGET_BUILDER_H
