/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_STRING_H
#define HAM_DATA_STRING_H


#include <list>
#include <string>


typedef std::string String;
typedef std::list<String> StringList;


extern const StringList kTrueStringList;
extern const StringList kFalseStringList;


#endif	// HAM_DATA_STRING_H
