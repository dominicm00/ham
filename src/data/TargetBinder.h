/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_TARGET_BINDER_H
#define HAM_DATA_TARGET_BINDER_H


#include "data/String.h"


namespace ham {
namespace data {


class FileStatus;
class Target;
class VariableDomain;


class TargetBinder {
public:
	static	void				Bind(const VariableDomain& globalVariables,
									const Target* target, String& _boundPath,
									FileStatus& _fileStatus);
};


}	// namespace data
}	// namespace ham


#endif	// HAM_DATA_TARGET_BINDER_H
