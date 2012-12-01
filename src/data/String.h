/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_STRING_H
#define HAM_DATA_STRING_H


#include <list>
#include <string>

#include "StringBuffer.h"


namespace ham {
namespace data {


typedef std::string String;
typedef std::list<String> StringList;


extern const StringList kTrueStringList;
extern const StringList kFalseStringList;


} // namespace data


using data::String;
using data::StringList;


} // namespace ham


#endif	// HAM_DATA_STRING_H
