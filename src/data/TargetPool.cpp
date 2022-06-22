/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "data/TargetPool.hpp"

namespace ham
{
namespace data
{

TargetPool::TargetPool() {}

TargetPool::~TargetPool() {}

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

void
TargetPool::LookupOrCreate(const StringList& names, TargetList& _targets)
{
	for (StringList::Iterator it = names.GetIterator(); it.HasNext();)
		_targets.push_back(LookupOrCreate(it.Next()));
}

} // namespace data
} // namespace ham
