/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_UTIL_CONSTANTS_HPP
#define HAM_UTIL_CONSTANTS_HPP

#include <stddef.h>

namespace ham::util
{

extern const char* const kJamfileName;

static const size_t kIncludeDepthLimit = 256;
static const size_t kRuleCallDepthLimit = 1024;

} // namespace ham::util

#endif // HAM_UTIL_CONSTANTS_HPP
