/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_PROCESS_PROCESS_HPP
#define HAM_PROCESS_PROCESS_HPP

#include "platform/PlatformProcessDelegate.hpp"

namespace ham::process
{

class ChildInfo;

class Process
{
  public:
	typedef PlatformProcessDelegate::Id Id;

  public:
	Process();

	void Unset() { fPlatformDelegate.Unset(); }

	bool IsValid() const { return fPlatformDelegate.IsValid(); }

	bool Launch(const char* command,
				const char* const* arguments,
				size_t argumentCount);

	Id GetId() const { return fPlatformDelegate.GetId(); }

	static bool WaitForChild(ChildInfo& _childInfo);

  private:
	PlatformProcessDelegate fPlatformDelegate;
};

} // namespace ham::process

#endif // HAM_PROCESS_PROCESS_HPP
