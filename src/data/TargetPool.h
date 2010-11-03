/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_TARGET_POOL_H
#define HAM_DATA_TARGET_POOL_H


#include <map>

#include "data/Target.h"


namespace data {


class TargetPool {
public:
								TargetPool();
								~TargetPool();

	inline	Target*				Lookup(const String& name);
	inline	Target*				LookupOrCreate(const String& name);

private:
			typedef std::map<String, Target> TargetMap;

private:
			TargetMap			fTargets;
};


Target*
TargetPool::Lookup(const String& name)
{
	TargetMap::iterator it = fTargets.find(name);
	return it == fTargets.end() ? NULL : &it->second;
}


Target*
TargetPool::LookupOrCreate(const String& name)
{
	// look up
	TargetMap::iterator it = fTargets.find(name);
	if (it != fTargets.end())
		return &it->second;

	// not found -- create
	Target& target = fTargets[name];
	target.SetName(name);
	return &target;
}


} // namespace data


#endif	// HAM_DATA_TARGET_POOL_H
