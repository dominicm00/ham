#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/ParseUtils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<VariableAssignment>;

TEST_CASE("Variable assignment does not consume whitespace", "[grammar]")
{
	auto stmt = GENERATE(" X = a ;", "X = a ; ");
	INFO(stmt);
	REQUIRE_THROWS(parse(stmt));
}

TEST_CASE("Variable assignment requires semicolon", "[grammar]")
{
	auto stmt = GENERATE("X = a", "local X on B = C", "X ?= a b c");
	INFO(stmt);
	REQUIRE_THROWS(parse(stmt));
}

TEST_CASE("Variable assignment requires valid operator", "[grammar]")
{
	auto stmt = GENERATE("X ? a ;", "X =& a ;", "X &= a ;");
	INFO(stmt);
	REQUIRE_THROWS(parse(stmt));
}

TEST_CASE(
	"Variable assignment cannot be both scope and target local",
	"[grammar]"
)
{
	REQUIRE_THROWS(parse("local X on target = a ;"));
}

TEST_CASE("Variable assignment requires targets after 'on'", "[grammar]")
{
	REQUIRE_THROWS(parse("X on = a ;"));
}

TEST_CASE("Global variable assignment", "[grammar]")
{
	REQUIRE_PARSE(
		"X = a b ;",
		T<VariableAssignment>(
			{T<Identifier>("X"), T<AssignmentOperator>("="), T<List>("a b")}
		)
	);
	REQUIRE_PARSE(
		"X ?= a ;",
		T<VariableAssignment>(
			{T<Identifier>("X"), T<AssignmentOperator>("?="), T<List>("a")}
		)
	);
	REQUIRE_PARSE(
		"X += ;",
		T<VariableAssignment>({T<Identifier>("X"), T<AssignmentOperator>("+=")})
	);
}

TEST_CASE("Local variable assignment", "[grammar]")
{
	REQUIRE_PARSE(
		"local X = a b ;",
		T<VariableAssignment>(
			{T<LocalVariableModifier>(),
			 T<Identifier>("X"),
			 T<AssignmentOperator>("="),
			 T<List>("a b")}
		)
	);
}

TEST_CASE("Target variable assignment", "[grammar]")
{
	REQUIRE_PARSE(
		"X on t1 t2 = a b ;",
		T<VariableAssignment>(
			{T<Identifier>("X"),
			 T<List>("t1 t2"),
			 T<AssignmentOperator>("="),
			 T<List>("a b")}
		)
	);
}
} // namespace ham::tests
