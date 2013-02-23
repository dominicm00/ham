/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_MAKE_MAKE_EXCEPTION_H
#define HAM_MAKE_MAKE_EXCEPTION_H


#include "util/Exception.h"


namespace ham {
namespace make {


class MakeException : public util::Exception {
public:
	MakeException(const std::string& message = std::string())
		:
		util::Exception(message)
	{
	}
};


}	// namespace make
}	// namespace ham


#endif	// HAM_MAKE_MAKE_EXCEPTION_H
