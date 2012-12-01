/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "test/Test.h"

#include "test/TestSuite.h"


namespace ham {
namespace test {


Test::Test(const std::string& name)
	:
	fName(name),
	fTestSuite(NULL)
{
}


Test::~Test()
{
}


std::string
Test::FullyQualifiedName() const
{
	if (fTestSuite == NULL)
		return fName;

	std::string name = fTestSuite->FullyQualifiedName();
	if (name.empty())
		return fName;

	return name + "::" + fName;
}


}	// namespace test
}	// namespace ham
