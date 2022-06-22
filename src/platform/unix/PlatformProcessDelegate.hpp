/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_PLATFORM_UNIX_PLATFORM_PROCESS_DELEGATE_HPP
#define HAM_PLATFORM_UNIX_PLATFORM_PROCESS_DELEGATE_HPP

#include <unistd.h>

namespace ham::process
{

class ChildInfo;

class PlatformProcessDelegate
{
  public:
	typedef pid_t Id;

  public:
	PlatformProcessDelegate();

	void Unset();

	bool IsValid() const { return fPid >= 0; }

	bool Launch(const char* command,
				const char* const* arguments,
				size_t argumentCount);

	Id GetId() const { return fPid; }

	static bool WaitForChild(ChildInfo& _childInfo);

  private:
	pid_t fPid;
};

} // namespace ham::process

#endif // HAM_PLATFORM_UNIX_PLATFORM_PROCESS_DELEGATE_HPP
