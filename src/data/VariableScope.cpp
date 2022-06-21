/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "VariableScope.h"

namespace ham
{
namespace data
{

StringList*
VariableScope::Lookup(const String& variable) const
{
	const VariableScope* scope = this;
	while (scope != nullptr) {
		StringList* value = scope->fDomain.Lookup(variable);
		if (value != nullptr)
			return value;

		scope = scope->fParent;
	}

	return nullptr;
}

} // namespace data
} // namespace ham
