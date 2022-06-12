/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_VARIABLE_SCOPE_H
#define HAM_DATA_VARIABLE_SCOPE_H

#include "data/VariableDomain.h"

namespace ham
{
namespace data
{

class VariableScope
{
  public:
	inline VariableScope(VariableDomain& domain, VariableScope* parent);

	VariableScope* Parent() const { return fParent; }

	StringList* Lookup(const String& variable) const;
	inline void Set(const String& variable, const StringList& value);

  private:
	VariableDomain& fDomain;
	VariableScope* fParent;
};

VariableScope::VariableScope(VariableDomain& domain, VariableScope* parent)
	: fDomain(domain),
	  fParent(parent)
{
}

void
VariableScope::Set(const String& variable, const StringList& value)
{
	fDomain.Set(variable, value);
}

} // namespace data
} // namespace ham

#endif // HAM_DATA_VARIABLE_SCOPE_H
