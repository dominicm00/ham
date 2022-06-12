/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_PLATFORM_UNIX_PLATFORM_PROCESS_DELEGATE_H
#define HAM_PLATFORM_UNIX_PLATFORM_PROCESS_DELEGATE_H

#include <unistd.h>

namespace ham
{
namespace process
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

} // namespace process
} // namespace ham

#endif // HAM_PLATFORM_UNIX_PLATFORM_PROCESS_DELEGATE_H
