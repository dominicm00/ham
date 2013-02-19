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
	fBoundPath()
{
}


MakeTarget::~MakeTarget()
{
}


} // namespace data
} // namespace ham
