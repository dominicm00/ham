/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_TARGET_CONTAINERS_H
#define HAM_DATA_TARGET_CONTAINERS_H

#include "util/SequentialSet.h"

namespace ham
{
namespace data
{

class Target;

typedef util::SequentialSet<Target*> TargetSet;
typedef std::vector<Target*> TargetList;

} // namespace data
} // namespace ham

#endif // HAM_DATA_TARGET_CONTAINERS_H
