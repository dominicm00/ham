/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "VariableScope.h"


namespace data {


StringList*
VariableScope::Lookup(const String& variable) const
{
	const VariableScope* scope = this;
	while (scope != NULL) {
		StringList* value = scope->fDomain.Lookup(variable);
		if (value != NULL)
			return value;

		scope = scope->fParent;
	}

	return NULL;
}


} // namespace data
