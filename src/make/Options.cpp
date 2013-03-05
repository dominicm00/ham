/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "make/Options.h"


namespace ham {
namespace make {


Options::Options()
	:
	fJambaseFile(),
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


} // namespace make
} // namespace ham

