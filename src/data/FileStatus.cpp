/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "data/FileStatus.hpp"

namespace ham
{
namespace data
{

FileStatus::FileStatus()
	: fType(NONE),
	  fLastModifiedTime()
{
}

FileStatus::FileStatus(Type type, const Time& lastModifiedTime)
	: fType(type),
	  fLastModifiedTime(lastModifiedTime)
{
}

} // namespace data
} // namespace ham
