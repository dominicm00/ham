/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_MAKE_TARGET_H
#define HAM_DATA_MAKE_TARGET_H


#include "data/FileStatus.h"
#include "data/Target.h"


namespace ham {
namespace data {


class MakeTarget;

typedef std::set<MakeTarget*> MakeTargetSet;


class MakeTarget {
public:
								MakeTarget(Target* target);
								~MakeTarget();

			Target*				GetTarget() const
									{ return fTarget; }

			bool				IsBound() const
									{ return !fBoundPath.IsEmpty(); }
			String				BoundPath() const
									{ return fBoundPath; }
			void				SetBoundPath(const String& path)
									{ fBoundPath = path; }

			const FileStatus&	GetFileStatus() const
									{ return fFileStatus; }
			void				SetFileStatus(const FileStatus& fileStatus)
									{ fFileStatus = fileStatus; }

private:
			Target*				fTarget;
			String				fBoundPath;
			FileStatus			fFileStatus;
};


} // namespace data
} // namespace ham


#endif	// HAM_DATA_MAKE_TARGET_H
