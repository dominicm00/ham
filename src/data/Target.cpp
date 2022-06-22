/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "data/Target.hpp"

namespace ham
{
namespace data
{

Target::Target()
	: fName(),
	  fVariables(nullptr),
	  fFlags(0),
	  fDependencies(),
	  fIncludes(),
	  fActionsCalls()
{
}

Target::Target(const String& name)
	: fName(name),
	  fVariables(nullptr),
	  fFlags(0),
	  fDependencies(),
	  fIncludes(),
	  fActionsCalls()
{
}

Target::~Target()
{
	for (std::vector<RuleActionsCall*>::iterator it = fActionsCalls.begin();
		 it != fActionsCalls.end();
		 ++it) {
		(*it)->ReleaseReference();
	}

	delete fVariables;
}

} // namespace data
} // namespace ham
