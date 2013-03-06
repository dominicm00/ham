/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/Jambase.h"


namespace ham {
namespace code {


const char* const kJambase =
#if 0
#	include "JambaseData.h"
#else
	"NotFile all ;\n"
	"include Jamfile ;\n"
#endif
	;


}	// namespace code
}	// namespace ham
