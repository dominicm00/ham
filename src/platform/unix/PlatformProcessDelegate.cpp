/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "platform/unix/PlatformProcessDelegate.hpp"

#include "process/ChildInfo.hpp"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

namespace ham::process
{

PlatformProcessDelegate::PlatformProcessDelegate()
	: fPid(-1)
{
}

void
PlatformProcessDelegate::Unset()
{
	fPid = -1;
}

bool
PlatformProcessDelegate::Launch(const char* command,
								const char* const* arguments,
								size_t /*argumentCount*/)
{
	Unset();

	// fork() and exec*()
	pid_t pid = fork();
	if (pid < 0) {
		fprintf(stderr, "Error: fork failed(): %s\n", strerror(errno));
		return false;
	}

	if (pid == 0) {
		// child process
		execv(command, (char* const*)arguments);
		fprintf(stderr, "Error: execv() failed: %s\n", strerror(errno));
		exit(1);
	}

	fPid = pid;
	return true;
}

/*static*/ bool
PlatformProcessDelegate::WaitForChild(ChildInfo& _childInfo)
{
	int status;
	pid_t pid = wait(&status);
	if (pid < 0)
		return false;

	if (WIFEXITED(status))
		_childInfo.fExitCode = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		_childInfo.fExitCode = 256;
	else
		return false;

	_childInfo.fId = pid;
	_childInfo.fExited = true;
	return true;
}

} // namespace ham::process
