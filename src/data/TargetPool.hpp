/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_TARGET_POOL_HPP
#define HAM_DATA_TARGET_POOL_HPP

#include "data/Target.hpp"

#include <map>

namespace ham::data
{

class TargetPool
{
  public:
	TargetPool();
	~TargetPool();

	inline Target* Lookup(const String& name);
	Target* LookupOrCreate(const String& name);
	void LookupOrCreate(const StringList& names, TargetList& _targets);

  private:
	typedef std::map<String, Target> TargetMap;

  private:
	TargetMap fTargets;
};

Target*
TargetPool::Lookup(const String& name)
{
	TargetMap::iterator it = fTargets.find(name);
	return it == fTargets.end() ? nullptr : &it->second;
}

} // namespace ham::data

#endif // HAM_DATA_TARGET_POOL_HPP
