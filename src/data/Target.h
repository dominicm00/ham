/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_TARGET_H
#define HAM_DATA_TARGET_H


#include "data/VariableDomain.h"


namespace ham {
namespace data {


class Target {
public:
	inline						Target();
	inline						Target(const String& name);
								~Target();

	inline	void				SetName(const String& name);
									// conceptually package private

	inline	VariableDomain*		Variables(bool create);

private:
			String				fName;
			VariableDomain*		fVariables;
};



Target::Target()
	:
	fName(),
	fVariables(NULL)
{
}


Target::Target(const String& name)
	:
	fName(name),
	fVariables(NULL)
{
}


void
Target::SetName(const String& name)
{
	fName = name;
}


VariableDomain*
Target::Variables(bool create)
{
	if (fVariables == NULL && create)
		fVariables = new VariableDomain;

	return fVariables;
}


} // namespace data
} // namespace ham


#endif	// HAM_DATA_TARGET_H
