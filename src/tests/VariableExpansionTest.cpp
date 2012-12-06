/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "VariableExpansionTest.h"

#include "code/Leaf.h"


namespace ham {
namespace tests {


VariableExpansionTest::VariableExpansionTest()
	:
	fGlobalVariables(NULL),
	fTargets(NULL),
	fEvaluationContext(NULL)
{
}


void
VariableExpansionTest::InitTestCase()
{
	fGlobalVariables = new data::VariableDomain;
	fTargets = new data::TargetPool;
	fEvaluationContext = new code::EvaluationContext(*fGlobalVariables,
		*fTargets);

	fGlobalVariables->Set("empty", StringList());
	fGlobalVariables->Set("single", MakeStringList("Skywalker"));
	fGlobalVariables->Set("double", MakeStringList("Darth", "Vader"));
	fGlobalVariables->Set("triple", MakeStringList("a", "b", "c"));
	fGlobalVariables->Set("triple2", MakeStringList("x", "y", "z"));
}


void
VariableExpansionTest::CleanupTestCase()
{
	delete fEvaluationContext;
	delete fTargets;
	delete fGlobalVariables;
}


void
VariableExpansionTest::None()
{
	HAM_TEST_EQUAL(_Evaluate(""), MakeStringList(""))
	HAM_TEST_EQUAL(_Evaluate("foo"), MakeStringList("foo"))
	HAM_TEST_EQUAL(_Evaluate("foo$tring"), MakeStringList("foo$tring"))
	HAM_TEST_EQUAL(_Evaluate("foo(bar)$tring"),
		MakeStringList("foo(bar)$tring"))
}


void
VariableExpansionTest::Simple()
{
	struct TestData {
		const char*	string;
		StringList	result;
	};

	const TestData testData[] = {
		{ "$(undefined)",			StringList() },
		{ "foo$(undefined)",		StringList() },
		{ "$(undefined)bar",		StringList() },
		{ "foo$(undefined)bar",		StringList() },
		{ "$(empty)",				StringList() },
		{ "foo$(empty)",			StringList() },
		{ "$(empty)bar",			StringList() },
		{ "foo$(empty)bar",			StringList() },
		{ "$(single)",				MakeStringList("Skywalker") },
		{ "foo$(single)",			MakeStringList("fooSkywalker") },
		{ "$(single)bar",			MakeStringList("Skywalkerbar") },
		{ "foo$(single)bar",		MakeStringList("fooSkywalkerbar") },
		{ "$(double)",				MakeStringList("Darth", "Vader") },
		{ "foo$(double)",			MakeStringList("fooDarth", "fooVader") },
		{ "$(double)bar",			MakeStringList("Darthbar", "Vaderbar") },
		{ "foo$(double)bar",
			MakeStringList("fooDarthbar", "fooVaderbar") },
		{ "$(triple)",				MakeStringList("a", "b", "c") },
		{ "foo$(triple)",			MakeStringList("fooa", "foob", "fooc") },
		{ "$(triple)bar",			MakeStringList("abar", "bbar", "cbar") },
		{ "foo$(triple)bar",
			MakeStringList("fooabar", "foobbar", "foocbar") },
		{ "$(undefined)(triple)",	StringList() },
		{ "$(triple)$(undefined)",	StringList() },
		{ "$(empty)$(triple)",		StringList() },
		{ "$(triple)$(empty)",		StringList() },
		{ "$(single)$(triple)",
			MakeStringList("Skywalkera", "Skywalkerb", "Skywalkerc") },
		{ "$(triple)$(single)",
			MakeStringList("aSkywalker", "bSkywalker", "cSkywalker") },
		{ "$(double)$(triple)",
			MakeStringList("Dartha", "Darthb", "Darthc", "Vadera", "Vaderb",
				"Vaderc") },
		{ "$(triple)$(double)",
			MakeStringList("aDarth", "aVader", "bDarth", "bVader", "cDarth",
				"cVader") },
		{ "1$(double)2$(triple)3",
			MakeStringList("1Darth2a3", "1Darth2b3", "1Darth2c3", "1Vader2a3",
				"1Vader2b3", "1Vader2c3") },
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++)
		HAM_TEST_EQUAL(_Evaluate(testData[i].string), testData[i].result)
}


void
VariableExpansionTest::Subscripts()
{
	struct TestData {
		const char*	string;
		StringList	result;
	};

	const TestData testData[] = {
		{ "$(undefined[1])",			StringList() },
		{ "$(empty[1])",				StringList() },
		{ "$(single[0])",				MakeStringList("Skywalker") },
		{ "$(single[1])",				MakeStringList("Skywalker") },
		{ "$(single[2])",				StringList() },
		{ "$(double[0])",				MakeStringList("Darth") },
		{ "$(double[1])",				MakeStringList("Darth") },
		{ "$(double[2])",				MakeStringList("Vader") },
		{ "$(double[3])",				StringList() },
		{ "$(triple[0])",				MakeStringList("a") },
		{ "$(triple[1])",				MakeStringList("a") },
		{ "$(triple[2])",				MakeStringList("b") },
		{ "$(triple[3])",				MakeStringList("c") },
		{ "$(triple[4])",				StringList() },
		{ "$(undefined[1-])",			StringList() },
		{ "$(empty[1-])",				StringList() },
		{ "$(single[0-])",				MakeStringList("Skywalker") },
		{ "$(single[1-])",				MakeStringList("Skywalker") },
		{ "$(single[2-])",				StringList() },
		{ "$(double[0-])",				MakeStringList("Darth", "Vader") },
		{ "$(double[1-])",				MakeStringList("Darth", "Vader") },
		{ "$(double[2-])",				MakeStringList("Vader") },
		{ "$(double[3-])",				StringList() },
		{ "$(triple[0-])",				MakeStringList("a", "b", "c") },
		{ "$(triple[1-])",				MakeStringList("a", "b", "c") },
		{ "$(triple[2-])",				MakeStringList("b", "c") },
		{ "$(triple[3-])",				MakeStringList("c") },
		{ "$(triple[4-])",				StringList() },
		{ "$(undefined[1-4])",			StringList() },
		{ "$(empty[1-4])",				StringList() },
		{ "$(single[0-1])",				MakeStringList("Skywalker") },
		{ "$(single[1-1])",				MakeStringList("Skywalker") },
		{ "$(single[1-2])",				MakeStringList("Skywalker") },
		{ "$(single[2-4])",				StringList() },
		{ "$(double[0-1])",				MakeStringList("Darth") },
		{ "$(double[0-2])",				MakeStringList("Darth", "Vader") },
		{ "$(double[1-1])",				MakeStringList("Darth") },
		{ "$(double[1-2])",				MakeStringList("Darth", "Vader") },
		{ "$(double[1-3])",				MakeStringList("Darth", "Vader") },
		{ "$(double[2-2])",				MakeStringList("Vader") },
		{ "$(double[2-3])",				MakeStringList("Vader") },
		{ "$(double[3-3])",				StringList() },
		{ "$(double[3-4])",				StringList() },
		{ "$(triple[0-1])",				MakeStringList("a") },
		{ "$(triple[1-1])",				MakeStringList("a") },
		{ "$(triple[1-2])",				MakeStringList("a", "b") },
		{ "$(triple[1-3])",				MakeStringList("a", "b", "c") },
		{ "$(triple[1-4])",				MakeStringList("a", "b", "c") },
		{ "$(triple[2-2])",				MakeStringList("b") },
		{ "$(triple[2-3])",				MakeStringList("b", "c") },
		{ "$(triple[2-4])",				MakeStringList("b", "c") },
		{ "$(triple[3-4])",				MakeStringList("c") },
		{ "$(triple[4-4])",				StringList() },
		{ "1$(double)2$(triple[2-3])3",
			MakeStringList("1Darth2b3", "1Darth2c3", "1Vader2b3",
				"1Vader2c3") },
		{ "1$(double[4])2$(triple[2-3])3",
			StringList() },
		{ "$(triple2[2-3])$(triple[1-2])",
			MakeStringList("ya", "yb", "za", "zb") },
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++)
		HAM_TEST_EQUAL(_Evaluate(testData[i].string), testData[i].result)
}


void
VariableExpansionTest::Operations()
{
	struct TestData {
		const char*	string;
		StringList	result;
	};

	const TestData testData[] = {
		{ "$(undefined:E=foo)",			MakeStringList("foo") },
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++)
		HAM_TEST_EQUAL(_Evaluate(testData[i].string), testData[i].result)
}


StringList
VariableExpansionTest::_Evaluate(const String& string)
{
	code::Leaf leaf(string);
	return leaf.Evaluate(*fEvaluationContext);
}


} // namespace tests
} // namespace ham
