/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "make/MakeTarget.h"

namespace ham
{
namespace make
{

MakeTarget::MakeTarget(const data::Target* target)
	: fTarget(target),
	  fBoundPath(),
	  fOriginalTime(),
	  fTime(),
	  fLeafTime(),
	  fFileExists(false),
	  fDependencies(),
	  fIncludes(),
	  fParents(),
	  fProcessingState(UNPROCESSED),
	  fState(UP_TO_DATE),
	  fFate(KEEP),
	  fMakeState(PENDING),
	  fPendingDependencyCount(0)
{
}

MakeTarget::~MakeTarget() {}

void
MakeTarget::SetFileStatus(const data::FileStatus& fileStatus)
{
	fFileExists = fileStatus.GetType() != data::FileStatus::NONE;
	fOriginalTime = fFileExists ? fileStatus.LastModifiedTime() : data::Time();
}

} // namespace make
} // namespace ham
