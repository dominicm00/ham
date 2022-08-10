#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<variable>;

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
	REQUIRE_PARSE("$(Variable)", T<variable>({T<identifier>("Variable")}));
}

TEST_CASE("Nested variables", "[grammar]")
{
	REQUIRE_PARSE(
		"$(a$(b))",
		T<variable>({T<identifier>(
			{T<id_char>("a"), T<variable>({T<identifier>("b")})}
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
		T<variable>({T<identifier>("var"), T<subscript>({T<identifier>("3")})})
	);
}

TEST_CASE("Start-only range subscripts", "[grammar]")
{
	REQUIRE_PARSE(
		"$(var[3-])",
		T<variable>({T<identifier>("var"), T<subscript>({T<identifier>("3-")})})
	);
}

TEST_CASE("Range subscripts", "[grammar]")
{
	REQUIRE_PARSE(
		"$(var[3-5])",
		T<variable>({T<identifier>("var"), T<subscript>({T<identifier>("3-5")})}
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

TEST_CASE("Variable selectors", "[grammar]")
{
	REQUIRE_PARSE(
		"$(X:GB)",
		T<variable>(
			{T<identifier>("X"),
			 T<variable_selector>("G"),
			 T<variable_selector>("B")}
		)
	);

	REQUIRE_PARSE(
		"$(X:G)",
		{T<variable>({T<identifier>("X"), T<variable_selector>("G")})}
	);
}

TEST_CASE("Variable modifiers", "[grammar]")
{
	REQUIRE_PARSE(
		"$(X:G=<grist>)",
		T<variable>(
			{T<identifier>("X"),
			 T<variable_replacer>(
				 {T<variable_selector>("G"), T<leaf>("<grist>")}
			 )}
		)
	);
}

TEST_CASE("Mixed modifiers", "[grammar]")
{
	REQUIRE_PARSE(
		"$(X:GB=a:D)",
		T<variable>(
			{T<identifier>("X"),
			 T<variable_selector>("G"),
			 T<variable_replacer>({T<variable_selector>("B"), T<leaf>("a")}),
			 T<variable_selector>("D")}
		)
	);
}

} // namespace ham::tests
