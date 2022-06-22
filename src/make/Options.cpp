/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "make/Options.hpp"

namespace ham::make
{

Options::Options()
	: fRulesetFile(),
	  fActionsOutputFile(),
	  fDryRun(false),
	  fPrintMakeTree(false),
	  fPrintActions(false),
	  fPrintCommands(false),
	  fJobCount(1),
	  fBuildFromNewest(false),
	  fQuitOnError(false)
{
}

} // namespace ham::make
