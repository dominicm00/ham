/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_VARIABLE_DOMAIN_HPP
#define HAM_DATA_VARIABLE_DOMAIN_HPP

#include <map>

#include "StringList.hpp"

namespace ham
{
namespace data
{

class VariableDomain
{
  public:
	inline VariableDomain();

	inline const StringList* Lookup(const String& variable) const;
	inline StringList* Lookup(const String& variable);
	inline StringList& LookupOrCreate(const String& variable);
	inline void Set(const String& variable, const StringList& value);

  private:
	typedef std::map<String, StringList> VariableMap;

  private:
	VariableMap fVariables;
};

VariableDomain::VariableDomain() {}

const StringList*
VariableDomain::Lookup(const String& variable) const
{
	VariableMap::const_iterator it = fVariables.find(variable);
	return it == fVariables.end() ? nullptr : &it->second;
}

StringList*
VariableDomain::Lookup(const String& variable)
{
	VariableMap::iterator it = fVariables.find(variable);
	return it == fVariables.end() ? nullptr : &it->second;
}

StringList&
VariableDomain::LookupOrCreate(const String& variable)
{
	return fVariables[variable];
}

void
VariableDomain::Set(const String& variable, const StringList& value)
{
	fVariables[variable] = value;
}

} // namespace data
} // namespace ham

#endif // HAM_DATA_VARIABLE_DOMAIN_HPP
