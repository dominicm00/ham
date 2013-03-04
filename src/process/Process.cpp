/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "process/Process.h"


namespace ham {
namespace process {


class ChildInfo;


Process::Process()
	:
	fPlatformDelegate()
{

}

bool
Process::Launch(const char* command, const char* const* arguments,
	size_t argumentCount)
{
	return fPlatformDelegate.Launch(command, arguments, argumentCount);
}


/*static*/ bool
Process::WaitForChild(ChildInfo& _childInfo)
{
	return PlatformProcessDelegate::WaitForChild(_childInfo);
}


} // namespace process
} // namespace ham
