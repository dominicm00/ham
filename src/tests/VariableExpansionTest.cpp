/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "VariableExpansionTest.h"

#include "code/Leaf.h"

namespace ham
{
namespace tests
{

VariableExpansionTest::VariableExpansionTest()
	: fGlobalVariables(nullptr),
	  fTargets(nullptr),
	  fEvaluationContext(nullptr)
{
}

void
VariableExpansionTest::InitTestCase()
{
	fGlobalVariables = new data::VariableDomain;
	fTargets = new data::TargetPool;
	fEvaluationContext =
		new code::EvaluationContext(*fGlobalVariables, *fTargets);

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
VariableExpansionTest::PathSelectorsEmpty()
{
	struct TestData {
		const char* string;
		StringList result;
	};

	const TestData testData[] = {
		{"$(empty:G)", StringList()},		{"$(empty:R)", StringList()},
		{"$(empty:P)", StringList()},		{"$(empty:D)", StringList()},
		{"$(empty:B)", StringList()},		{"$(empty:S)", StringList()},
		{"$(empty:M)", StringList()},		{"$(empty:GR)", StringList()},
		{"$(empty:GP)", StringList()},		{"$(empty:GD)", StringList()},
		{"$(empty:GB)", StringList()},		{"$(empty:GS)", StringList()},
		{"$(empty:GM)", StringList()},		{"$(empty:RP)", StringList()},
		{"$(empty:RD)", StringList()},		{"$(empty:RB)", StringList()},
		{"$(empty:RS)", StringList()},		{"$(empty:RM)", StringList()},
		{"$(empty:PD)", StringList()},		{"$(empty:PB)", StringList()},
		{"$(empty:PS)", StringList()},		{"$(empty:PM)", StringList()},
		{"$(empty:DB)", StringList()},		{"$(empty:DS)", StringList()},
		{"$(empty:DM)", StringList()},		{"$(empty:BS)", StringList()},
		{"$(empty:BM)", StringList()},		{"$(empty:SM)", StringList()},
		{"$(empty:GRP)", StringList()},		{"$(empty:GRD)", StringList()},
		{"$(empty:GRB)", StringList()},		{"$(empty:GRS)", StringList()},
		{"$(empty:GRM)", StringList()},		{"$(empty:GPD)", StringList()},
		{"$(empty:GPB)", StringList()},		{"$(empty:GPS)", StringList()},
		{"$(empty:GPM)", StringList()},		{"$(empty:GDB)", StringList()},
		{"$(empty:GDS)", StringList()},		{"$(empty:GDM)", StringList()},
		{"$(empty:GBS)", StringList()},		{"$(empty:GBM)", StringList()},
		{"$(empty:GSM)", StringList()},		{"$(empty:RPD)", StringList()},
		{"$(empty:RPB)", StringList()},		{"$(empty:RPS)", StringList()},
		{"$(empty:RPM)", StringList()},		{"$(empty:RDB)", StringList()},
		{"$(empty:RDS)", StringList()},		{"$(empty:RDM)", StringList()},
		{"$(empty:RBS)", StringList()},		{"$(empty:RBM)", StringList()},
		{"$(empty:RSM)", StringList()},		{"$(empty:PDB)", StringList()},
		{"$(empty:PDS)", StringList()},		{"$(empty:PDM)", StringList()},
		{"$(empty:PBS)", StringList()},		{"$(empty:PBM)", StringList()},
		{"$(empty:PSM)", StringList()},		{"$(empty:DBS)", StringList()},
		{"$(empty:DBM)", StringList()},		{"$(empty:DSM)", StringList()},
		{"$(empty:BSM)", StringList()},		{"$(empty:GRPD)", StringList()},
		{"$(empty:GRPB)", StringList()},	{"$(empty:GRPS)", StringList()},
		{"$(empty:GRPM)", StringList()},	{"$(empty:GRDB)", StringList()},
		{"$(empty:GRDS)", StringList()},	{"$(empty:GRDM)", StringList()},
		{"$(empty:GRBS)", StringList()},	{"$(empty:GRBM)", StringList()},
		{"$(empty:GRSM)", StringList()},	{"$(empty:GPDB)", StringList()},
		{"$(empty:GPDS)", StringList()},	{"$(empty:GPDM)", StringList()},
		{"$(empty:GPBS)", StringList()},	{"$(empty:GPBM)", StringList()},
		{"$(empty:GPSM)", StringList()},	{"$(empty:GDBS)", StringList()},
		{"$(empty:GDBM)", StringList()},	{"$(empty:GDSM)", StringList()},
		{"$(empty:GBSM)", StringList()},	{"$(empty:RPDB)", StringList()},
		{"$(empty:RPDS)", StringList()},	{"$(empty:RPDM)", StringList()},
		{"$(empty:RPBS)", StringList()},	{"$(empty:RPBM)", StringList()},
		{"$(empty:RPSM)", StringList()},	{"$(empty:RDBS)", StringList()},
		{"$(empty:RDBM)", StringList()},	{"$(empty:RDSM)", StringList()},
		{"$(empty:RBSM)", StringList()},	{"$(empty:PDBS)", StringList()},
		{"$(empty:PDBM)", StringList()},	{"$(empty:PDSM)", StringList()},
		{"$(empty:PBSM)", StringList()},	{"$(empty:DBSM)", StringList()},
		{"$(empty:GRPDB)", StringList()},	{"$(empty:GRPDS)", StringList()},
		{"$(empty:GRPDM)", StringList()},	{"$(empty:GRPBS)", StringList()},
		{"$(empty:GRPBM)", StringList()},	{"$(empty:GRPSM)", StringList()},
		{"$(empty:GRDBS)", StringList()},	{"$(empty:GRDBM)", StringList()},
		{"$(empty:GRDSM)", StringList()},	{"$(empty:GRBSM)", StringList()},
		{"$(empty:GPDBS)", StringList()},	{"$(empty:GPDBM)", StringList()},
		{"$(empty:GPDSM)", StringList()},	{"$(empty:GPBSM)", StringList()},
		{"$(empty:GDBSM)", StringList()},	{"$(empty:RPDBS)", StringList()},
		{"$(empty:RPDBM)", StringList()},	{"$(empty:RPDSM)", StringList()},
		{"$(empty:RPBSM)", StringList()},	{"$(empty:RDBSM)", StringList()},
		{"$(empty:PDBSM)", StringList()},	{"$(empty:GRPDBS)", StringList()},
		{"$(empty:GRPDBM)", StringList()},	{"$(empty:GRPDSM)", StringList()},
		{"$(empty:GRPBSM)", StringList()},	{"$(empty:GRDBSM)", StringList()},
		{"$(empty:GPDBSM)", StringList()},	{"$(empty:RPDBSM)", StringList()},
		{"$(empty:GRPDBSM)", StringList()},
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		HAM_TEST_ADD_INFO(
			HAM_TEST_EQUAL(_Evaluate(testData[i].string), testData[i].result),
			"string: %s",
			testData[i].string)
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
