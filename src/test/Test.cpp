/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "test/Test.hpp"

#include "test/TestSuite.hpp"

namespace ham::test
{

Test::Test(const std::string& name)
	: fName(name),
	  fTestSuite(nullptr)
{
}

Test::~Test() {}

std::string
Test::FullyQualifiedName() const
{
	if (fTestSuite == nullptr)
		return fName;

	std::string name = fTestSuite->FullyQualifiedName();
	if (name.empty())
		return fName;

	return name + "::" + fName;
}

} // namespace ham::test
