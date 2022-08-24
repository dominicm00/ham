#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/ParseUtils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<Variable>;

/*
 * Identifier only
 */
TEST_CASE("Variables are not identifiers", "[grammar]")
{
	auto id = GENERATE("Id", "Id1", "1id2", "UPPERCASE", "CamelCase");
	REQUIRE_THROWS(parse(id));
}

TEST_CASE("Variable must be surrounded by '$()'", "[grammar]")
{
	auto str = GENERATE("missingbegin)", "(missingdollar)", "$(bad)suffix");
	REQUIRE_THROWS(parse(str));
}

TEST_CASE("Unmatched variable sequence errors '$()'", "[grammar]")
{
	auto str = GENERATE("$(missingend", "$( missingend[3-4]", "$onlystart");
	REQUIRE_THROWS(parse(str));
}

TEST_CASE("Whitespace is accepted between '$(  )'", "[grammar]")
{
	auto id = GENERATE("$( Whitespace )", "$(UnevenF )", "$( UnevenB)");
	REQUIRE(parse(id));
}

TEST_CASE("Identifier is only child of simple variable", "[grammar]")
{
	REQUIRE_PARSE("$(Variable)", T<Variable>({T<Identifier>("Variable")}));
}

TEST_CASE("Nested variables", "[grammar]")
{
	REQUIRE_PARSE(
		"$(a$(b))",
		T<Variable>({T<Identifier>(
			{T<IdString>("a"), T<Variable>({T<Identifier>("b")})}
		)})
	);
}

/*
 * Subscripts
 */
TEST_CASE("Single element subscripts", "[grammar]")
{
	REQUIRE_PARSE(
		"$(var[3])",
		T<Variable>({T<Identifier>("var"), T<Subscript>({T<Identifier>("3")})})
	);
}

TEST_CASE("Start-only range subscripts", "[grammar]")
{
	REQUIRE_PARSE(
		"$(var[3-])",
		T<Variable>({T<Identifier>("var"), T<Subscript>({T<Identifier>("3-")})})
	);
}

TEST_CASE("Range subscripts", "[grammar]")
{
	REQUIRE_PARSE(
		"$(var[3-5])",
		T<Variable>({T<Identifier>("var"), T<Subscript>({T<Identifier>("3-5")})}
		)
	);
}

/**
 * Modifiers
 */
TEST_CASE("Variable modifier clause requires arguments", "[grammar]")
{
	REQUIRE_THROWS(parse("$(X:)"));
}

TEST_CASE("Variable selectors, one", "[grammar]")
{
	REQUIRE_PARSE(
		"$(X:G)",
		{T<Variable>({T<Identifier>("X"), T<VariableSelector>("G")})}
	);
}

TEST_CASE("Variable selectors, two", "[grammar]")
{
	auto var = GENERATE("$(X:GB)", "$(X:G:B)");
	REQUIRE_PARSE(
		var,
		T<Variable>(
			{T<Identifier>("X"),
			 T<VariableSelector>("G"),
			 T<VariableSelector>("B")}
		)
	);
}

TEST_CASE("Variable selectors, three", "[grammar]")
{
	auto var = GENERATE("$(X:DBS)", "$(X:DB:S)", "$(X:D:BS)", "$(X:D:B:S)");
	REQUIRE_PARSE(
		var,
		T<Variable>(
			{T<Identifier>("X"),
			 T<VariableSelector>("D"),
			 T<VariableSelector>("B"),
			 T<VariableSelector>("S")}
		)
	);
}

TEST_CASE("Variable replacers", "[grammar]")
{
	REQUIRE_PARSE(
		"$(X:G=grist)",
		T<Variable>(
			{T<Identifier>("X"),
			 T<VariableReplacer>({T<VariableSelector>("G"), T<Leaf>("grist")})}
		)
	);

	REQUIRE_PARSE(
		"$(X:G='a b':B=\"x \"y)",
		T<Variable>(
			{T<Identifier>("X"),
			 T<VariableReplacer>({T<VariableSelector>("G"), T<Leaf>("'a b'")}),
			 T<VariableReplacer>({T<VariableSelector>("B"), T<Leaf>("\"x \"y")}
			 )}
		)
	);
}

TEST_CASE("Mixed modifiers", "[grammar]")
{
	REQUIRE_PARSE(
		"$(X:GB=a:D)",
		T<Variable>(
			{T<Identifier>("X"),
			 T<VariableSelector>("G"),
			 T<VariableReplacer>({T<VariableSelector>("B"), T<Leaf>("a")}),
			 T<VariableSelector>("D")}
		)
	);
}

} // namespace ham::tests
