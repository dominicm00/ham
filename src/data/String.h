/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_STRING_H
#define HAM_DATA_STRING_H


#include <list>
#include <string>


namespace data {


typedef std::string String;
typedef std::list<String> StringList;


extern const StringList kTrueStringList;
extern const StringList kFalseStringList;


} // namespace data


// TODO: Remove those!
using data::String;
using data::StringList;
using data::kTrueStringList;
using data::kFalseStringList;


#endif	// HAM_DATA_STRING_H
