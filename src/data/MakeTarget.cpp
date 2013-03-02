/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "data/MakeTarget.h"


namespace ham {
namespace data {


MakeTarget::MakeTarget(Target* target)
	:
	fTarget(target),
	fBoundPath(),
	fTime(),
	fLeafTime(),
	fFileExists(false),
	fDependencies(),
	fIncludes(),
	fParents(),
	fState(UP_TO_DATE),
	fFate(UNPROCESSED),
	fMakeState(PENDING),
	fPendingDependencyCount(0)
{
}


MakeTarget::~MakeTarget()
{
}


void
MakeTarget::SetFileStatus(const FileStatus& fileStatus)
{
	fFileExists = fileStatus.GetType() != FileStatus::NONE;
	fTime = fFileExists ? fileStatus.LastModifiedTime() : Time();
}


} // namespace data
} // namespace ham
