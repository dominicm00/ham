/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_PROCESS_CHILD_INFO_HPP
#define HAM_PROCESS_CHILD_INFO_HPP

#include "process/Process.hpp"

namespace ham::process
{

struct ChildInfo {
	Process::Id fId;
	bool fExited;
	int fExitCode;
};

} // namespace ham::process

#endif // HAM_PROCESS_CHILD_INFO_HPP
