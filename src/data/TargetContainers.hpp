/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_TARGET_CONTAINERS_HPP
#define HAM_DATA_TARGET_CONTAINERS_HPP

#include "util/SequentialSet.hpp"

namespace ham::data
{

class Target;

typedef util::SequentialSet<Target*> TargetSet;
typedef std::vector<Target*> TargetList;

} // namespace ham::data

#endif // HAM_DATA_TARGET_CONTAINERS_HPP
