#include "behavior/Behavior.h"


namespace ham {
namespace behavior {


Behavior::Behavior(Compatibility compatibility)
	:
	fEchoTrailingSpace(ECHO_NO_TRAILING_SPACE),
	fPathRootReplacerSlash(PATH_ROOT_REPLACER_SLASH_IF_NEEDED)
{
	switch (compatibility) {
		case COMPATIBILITY_JAM:
			fEchoTrailingSpace = ECHO_TRAILING_SPACE;
			fPathRootReplacerSlash = PATH_ROOT_REPLACER_SLASH_ALWAYS;
			break;
		case COMPATIBILITY_BOOST_JAM:
			fPathRootReplacerSlash = PATH_ROOT_REPLACER_SLASH_AVOID_DUPLICATE;
			break;
		case COMPATIBILITY_HAM_1:
			break;
	}
}


}	// namespace behavior
}	// namespace ham
