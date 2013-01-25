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

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		HAM_TEST_ADD_INFO(
			HAM_TEST_EQUAL(_Evaluate(testData[i].string), testData[i].result),
			"string: %s", testData[i].string
		)
	}
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

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		HAM_TEST_ADD_INFO(
			HAM_TEST_EQUAL(_Evaluate(testData[i].string), testData[i].result),
			"string: %s", testData[i].string
		)
	}
}


void
VariableExpansionTest::Operations()
{
	struct TestData {
		const char*	string;
		StringList	result;
	};

	const TestData testData[] = {
		{ "$(undefined:E)",				MakeStringList("") },
		{ "$(undefined:E=)",			MakeStringList("") },
		{ "$(undefined:E=foo)",			MakeStringList("foo") },
		{ "$(empty:E)",					MakeStringList("") },
		{ "$(empty:E=)",				MakeStringList("") },
		{ "$(empty:E=foo)",				MakeStringList("foo") },
		{ "$(single:E)",				MakeStringList("Skywalker") },
		{ "$(single:E=)",				MakeStringList("Skywalker") },
		{ "$(single:E=foo)",			MakeStringList("Skywalker") },
		{ "$(double:E)",				MakeStringList("Darth", "Vader") },
		{ "$(double:E=)",				MakeStringList("Darth", "Vader") },
		{ "$(double:E=foo)",			MakeStringList("Darth", "Vader") },
		{ "$(triple:E)",				MakeStringList("a", "b", "c") },
		{ "$(triple:E=)",				MakeStringList("a", "b", "c") },
		{ "$(triple:E=foo)",			MakeStringList("a", "b", "c") },
		{ "$(undefined:J)",				StringList() },
		{ "$(undefined:J=)",			StringList() },
		{ "$(undefined:J=a)",			StringList() },
		{ "$(empty:J)",					StringList() },
		{ "$(empty:J=)",				StringList() },
		{ "$(empty:J=XYZ)",				StringList() },
		{ "$(single:J)",				MakeStringList("Skywalker") },
		{ "$(single:J=)",				MakeStringList("Skywalker") },
		{ "$(single:J=XYZ)",			MakeStringList("Skywalker") },
		{ "$(double:J)",				MakeStringList("DarthVader") },
		{ "$(double:J=)",				MakeStringList("DarthVader") },
		{ "$(double:J=XYZ)",			MakeStringList("DarthXYZVader") },
		{ "$(triple:J)",				MakeStringList("abc") },
		{ "$(triple:J=)",				MakeStringList("abc") },
		{ "$(triple:J=XYZ)",			MakeStringList("aXYZbXYZc") },
		{ "$(undefined:U)",				StringList() },
		{ "$(empty:U)",					StringList() },
		{ "$(single:U)",				MakeStringList("SKYWALKER") },
		{ "$(double:U)",				MakeStringList("DARTH", "VADER") },
		{ "$(triple:U)",				MakeStringList("A", "B", "C") },
		{ "$(undefined:L)",				StringList() },
		{ "$(empty:L)",					StringList() },
		{ "$(single:L)",				MakeStringList("skywalker") },
		{ "$(double:L)",				MakeStringList("darth", "vader") },
		{ "$(triple:L)",				MakeStringList("a", "b", "c") },
		{ "$(single:UL)",				MakeStringList("SKYWALKER") },
		{ "$(single:LU)",				MakeStringList("SKYWALKER") },
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		HAM_TEST_ADD_INFO(
			HAM_TEST_EQUAL(_Evaluate(testData[i].string), testData[i].result),
			"string: %s", testData[i].string
		)
	}
}

void VariableExpansionTest::PathSelectorsEmpty()
{
	struct TestData {
		const char*	string;
		StringList	result;
	};

	const TestData testData[] = {
		{ "$(empty:G)",				StringList() },
		{ "$(empty:R)",				StringList() },
		{ "$(empty:P)",				StringList() },
		{ "$(empty:D)",				StringList() },
		{ "$(empty:B)",				StringList() },
		{ "$(empty:S)",				StringList() },
		{ "$(empty:M)",				StringList() },
		{ "$(empty:GR)",			StringList() },
		{ "$(empty:GP)",			StringList() },
		{ "$(empty:GD)",			StringList() },
		{ "$(empty:GB)",			StringList() },
		{ "$(empty:GS)",			StringList() },
		{ "$(empty:GM)",			StringList() },
		{ "$(empty:RP)",			StringList() },
		{ "$(empty:RD)",			StringList() },
		{ "$(empty:RB)",			StringList() },
		{ "$(empty:RS)",			StringList() },
		{ "$(empty:RM)",			StringList() },
		{ "$(empty:PD)",			StringList() },
		{ "$(empty:PB)",			StringList() },
		{ "$(empty:PS)",			StringList() },
		{ "$(empty:PM)",			StringList() },
		{ "$(empty:DB)",			StringList() },
		{ "$(empty:DS)",			StringList() },
		{ "$(empty:DM)",			StringList() },
		{ "$(empty:BS)",			StringList() },
		{ "$(empty:BM)",			StringList() },
		{ "$(empty:SM)",			StringList() },
		{ "$(empty:GRP)",			StringList() },
		{ "$(empty:GRD)",			StringList() },
		{ "$(empty:GRB)",			StringList() },
		{ "$(empty:GRS)",			StringList() },
		{ "$(empty:GRM)",			StringList() },
		{ "$(empty:GPD)",			StringList() },
		{ "$(empty:GPB)",			StringList() },
		{ "$(empty:GPS)",			StringList() },
		{ "$(empty:GPM)",			StringList() },
		{ "$(empty:GDB)",			StringList() },
		{ "$(empty:GDS)",			StringList() },
		{ "$(empty:GDM)",			StringList() },
		{ "$(empty:GBS)",			StringList() },
		{ "$(empty:GBM)",			StringList() },
		{ "$(empty:GSM)",			StringList() },
		{ "$(empty:RPD)",			StringList() },
		{ "$(empty:RPB)",			StringList() },
		{ "$(empty:RPS)",			StringList() },
		{ "$(empty:RPM)",			StringList() },
		{ "$(empty:RDB)",			StringList() },
		{ "$(empty:RDS)",			StringList() },
		{ "$(empty:RDM)",			StringList() },
		{ "$(empty:RBS)",			StringList() },
		{ "$(empty:RBM)",			StringList() },
		{ "$(empty:RSM)",			StringList() },
		{ "$(empty:PDB)",			StringList() },
		{ "$(empty:PDS)",			StringList() },
		{ "$(empty:PDM)",			StringList() },
		{ "$(empty:PBS)",			StringList() },
		{ "$(empty:PBM)",			StringList() },
		{ "$(empty:PSM)",			StringList() },
		{ "$(empty:DBS)",			StringList() },
		{ "$(empty:DBM)",			StringList() },
		{ "$(empty:DSM)",			StringList() },
		{ "$(empty:BSM)",			StringList() },
		{ "$(empty:GRPD)",			StringList() },
		{ "$(empty:GRPB)",			StringList() },
		{ "$(empty:GRPS)",			StringList() },
		{ "$(empty:GRPM)",			StringList() },
		{ "$(empty:GRDB)",			StringList() },
		{ "$(empty:GRDS)",			StringList() },
		{ "$(empty:GRDM)",			StringList() },
		{ "$(empty:GRBS)",			StringList() },
		{ "$(empty:GRBM)",			StringList() },
		{ "$(empty:GRSM)",			StringList() },
		{ "$(empty:GPDB)",			StringList() },
		{ "$(empty:GPDS)",			StringList() },
		{ "$(empty:GPDM)",			StringList() },
		{ "$(empty:GPBS)",			StringList() },
		{ "$(empty:GPBM)",			StringList() },
		{ "$(empty:GPSM)",			StringList() },
		{ "$(empty:GDBS)",			StringList() },
		{ "$(empty:GDBM)",			StringList() },
		{ "$(empty:GDSM)",			StringList() },
		{ "$(empty:GBSM)",			StringList() },
		{ "$(empty:RPDB)",			StringList() },
		{ "$(empty:RPDS)",			StringList() },
		{ "$(empty:RPDM)",			StringList() },
		{ "$(empty:RPBS)",			StringList() },
		{ "$(empty:RPBM)",			StringList() },
		{ "$(empty:RPSM)",			StringList() },
		{ "$(empty:RDBS)",			StringList() },
		{ "$(empty:RDBM)",			StringList() },
		{ "$(empty:RDSM)",			StringList() },
		{ "$(empty:RBSM)",			StringList() },
		{ "$(empty:PDBS)",			StringList() },
		{ "$(empty:PDBM)",			StringList() },
		{ "$(empty:PDSM)",			StringList() },
		{ "$(empty:PBSM)",			StringList() },
		{ "$(empty:DBSM)",			StringList() },
		{ "$(empty:GRPDB)",			StringList() },
		{ "$(empty:GRPDS)",			StringList() },
		{ "$(empty:GRPDM)",			StringList() },
		{ "$(empty:GRPBS)",			StringList() },
		{ "$(empty:GRPBM)",			StringList() },
		{ "$(empty:GRPSM)",			StringList() },
		{ "$(empty:GRDBS)",			StringList() },
		{ "$(empty:GRDBM)",			StringList() },
		{ "$(empty:GRDSM)",			StringList() },
		{ "$(empty:GRBSM)",			StringList() },
		{ "$(empty:GPDBS)",			StringList() },
		{ "$(empty:GPDBM)",			StringList() },
		{ "$(empty:GPDSM)",			StringList() },
		{ "$(empty:GPBSM)",			StringList() },
		{ "$(empty:GDBSM)",			StringList() },
		{ "$(empty:RPDBS)",			StringList() },
		{ "$(empty:RPDBM)",			StringList() },
		{ "$(empty:RPDSM)",			StringList() },
		{ "$(empty:RPBSM)",			StringList() },
		{ "$(empty:RDBSM)",			StringList() },
		{ "$(empty:PDBSM)",			StringList() },
		{ "$(empty:GRPDBS)",		StringList() },
		{ "$(empty:GRPDBM)",		StringList() },
		{ "$(empty:GRPDSM)",		StringList() },
		{ "$(empty:GRPBSM)",		StringList() },
		{ "$(empty:GRDBSM)",		StringList() },
		{ "$(empty:GPDBSM)",		StringList() },
		{ "$(empty:RPDBSM)",		StringList() },
		{ "$(empty:GRPDBSM)",		StringList() },
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		HAM_TEST_ADD_INFO(
			HAM_TEST_EQUAL(_Evaluate(testData[i].string), testData[i].result),
			"string: %s", testData[i].string
		)
	}
}


StringList
VariableExpansionTest::_Evaluate(const String& string)
{
	code::Leaf leaf(string);
	return leaf.Evaluate(*fEvaluationContext);
}


} // namespace tests
} // namespace ham
