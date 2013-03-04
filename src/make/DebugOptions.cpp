/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "make/DebugOptions.h"


namespace ham {
namespace make {


DebugOptions::DebugOptions()
	:
	fDryRun(false),
	fPrintMakeTree(false),
	fPrintActions(false),
	fPrintCommands(false)
{
}


} // namespace make
} // namespace ham

