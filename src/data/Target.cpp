/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "data/Target.h"


namespace data {


Target::~Target()
{
	delete fVariables;
}


} // namespace data
