/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_PROCESS_CHILD_INFO_H
#define HAM_PROCESS_CHILD_INFO_H


#include "process/Process.h"


namespace ham {
namespace process {


struct ChildInfo {
				Process::Id		fId;
				bool			fExited;
				int				fExitCode;
};


} // namespace process
} // namespace ham


#endif	// HAM_PROCESS_CHILD_INFO_H
