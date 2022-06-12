/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_UTIL_CONSTANTS_H
#define HAM_UTIL_CONSTANTS_H

#include <stddef.h>

namespace ham
{
namespace util
{

extern const char* const kJamfileName;

static const size_t kIncludeDepthLimit = 256;
static const size_t kRuleCallDepthLimit = 1024;

} // namespace util
} // namespace ham

#endif // HAM_UTIL_CONSTANTS_H
