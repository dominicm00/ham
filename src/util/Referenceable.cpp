/*
 * Copyright 2005-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "util/Referenceable.h"

namespace ham
{
namespace util
{

Referenceable::Referenceable()
	: fReferenceCount(1)
{
}

Referenceable::~Referenceable() {}

int32_t
Referenceable::AcquireReference()
{
	int32_t previousReferenceCount = increment_reference_count(fReferenceCount);
	if (previousReferenceCount == 0)
		FirstReferenceAcquired();
	return previousReferenceCount;
}

int32_t
Referenceable::ReleaseReference()
{
	int32_t previousReferenceCount = decrement_reference_count(fReferenceCount);
	if (previousReferenceCount == 1)
		LastReferenceReleased();
	return previousReferenceCount;
}

void
Referenceable::FirstReferenceAcquired()
{
}

void
Referenceable::LastReferenceReleased()
{
	delete this;
}

} // namespace util
} // namespace ham
