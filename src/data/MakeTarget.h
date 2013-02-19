/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_MAKE_TARGET_H
#define HAM_DATA_MAKE_TARGET_H


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

private:
			Target*				fTarget;
			String				fBoundPath;
};


} // namespace data
} // namespace ham


#endif	// HAM_DATA_MAKE_TARGET_H
