/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "data/Target.h"


namespace ham {
namespace data {


Target::Target()
	:
	fName(),
	fVariables(NULL),
	fFlags(0),
	fDependencies(),
	fIncludes()
{
}


Target::Target(const String& name)
	:
	fName(name),
	fVariables(NULL),
	fFlags(0),
	fDependencies(),
	fIncludes()
{
}


Target::~Target()
{
	delete fVariables;
}


}	// namespace data
}	// namespace ham
