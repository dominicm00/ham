#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
PARSE_FUNCTIONS(variable);

/*
 * Identifier only
 */
TEST_CASE("Variables are not identifiers", "[grammar]")
{
	auto id = GENERATE("Id", "Id1", "1id2", "UPPERCASE", "CamelCase");
	REQUIRE_FALSE(parse(id));
}

TEST_CASE("Variable must be surrounded by '$()'", "[grammar]")
{
	auto id = GENERATE(
		"not$(surrounded)",
		"$(missingend",
		"missingbegin)",
		"(missingdollar)",
		"$(bad)suffix"
	);
	REQUIRE_FALSE(parse(id));
}

TEST_CASE("Whitespace is accepted between '$(  )'", "[grammar]")
{
	auto id = GENERATE("$( Whitespace )", "$(UnevenF )", "$( UnevenB)");
	REQUIRE(parse(id));
}

TEST_CASE("Identifier is only child of simple variable", "[grammar]")
{
	std::string str = "$(Variable)";
	auto var = decompose(parse(str), {0});
	check(var, T<variable>({T<identifier>("Variable")}));
}

/*
 * Subscripts
 */
TEST_CASE("Single element subscripts", "[grammar]")
{
	std::string str = "$(var[3])";
	auto var = decompose(parse(str), {0});
	check(
		var,
		T<variable>({T<identifier>("var"), T<subscript>({T<number>("3")})})
	);
}

TEST_CASE("Start-only range subscripts", "[grammar]")
{
	std::string str = "$(var[3-])";
	auto var = decompose(parse(str), {0});
	check(
		var,
		T<variable>(
			{T<identifier>("var"),
			 T<subscript>({T<number>("3"), T<end_subscript>()})}
		)
	);
}

TEST_CASE("Range subscripts", "[grammar]")
{
	std::string str = "$(var[3-5])";
	auto var = decompose(parse(str), {0});
	check(
		var,
		T<variable>(
			{T<identifier>("var"),
			 T<subscript>({T<number>("3"), T<number>("5")})}
		)
	);
}

} // namespace ham::tests
