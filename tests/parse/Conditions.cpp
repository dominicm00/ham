#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<condition>;

TEST_CASE("Condition is non-empty", "[grammar]") { REQUIRE_THROWS(parse("")); }

TEST_CASE("Leafs reduce to conditions", "[grammar]")
{
	std::string str = GENERATE("x", "$(x)", "'a string'");
	REQUIRE_PARSE(str, T<leaf>(str));
}

/**
 * Comparators
 */
TEST_CASE("Available comparators", "[grammar]")
{
	std::string comparator = GENERATE("=", "!=", "<", ">", "<=", ">=", "in");
	INFO(comparator);

	SECTION("Parse structure")
	{
		REQUIRE_PARSE(
			"1 " + comparator + " 2",
			T<leaf_comparator>(comparator, {T<leaf>("1"), T<leaf>("2")})
		);
	}

	SECTION("Accepts variables")
	{
		REQUIRE(parse("$(1) " + comparator + " $(2)"));
	}
}

TEST_CASE("Comparators are not associative", "[grammar]")
{
	auto exp = GENERATE("1 = 2 = 3", "3 < 4 > 5", "5 != 4 in 3");
	REQUIRE_THROWS(parse(exp));
}

TEST_CASE("Comparators don't accept conditionals", "[grammar]")
{
	auto exp = GENERATE("( 1 = 2 ) = 3", "4 = ( 5 = 6 )");
	REQUIRE_THROWS(parse(exp));
}

/**
 * Conditional operators
 */
TEST_CASE("Simple conjunction", "[grammar]")
{
	REQUIRE_PARSE("1 && 2", T<logical_and>({T<leaf>("1"), T<leaf>("2")}));
	REQUIRE_PARSE(
		"1 && 2 && 3",
		T<logical_and>(
			{T<leaf>("1"), T<logical_and>({T<leaf>("2"), T<leaf>("3")})}
		)
	);
}

TEST_CASE("Simple disjunction", "[grammar]")
{
	REQUIRE_PARSE("1 || 2", T<logical_or>({T<leaf>("1"), T<leaf>("2")}));
	REQUIRE_PARSE(
		"1 || 2 || 3",
		T<logical_or>(
			{T<leaf>("1"), T<logical_or>({T<leaf>("2"), T<leaf>("3")})}
		)
	);
}

TEST_CASE("Can't mix conjunctions and disjunctions", "[grammar]")
{
	auto exp = GENERATE(
		"1 && 2 || 3",
		"1 || 2 && 3",
		"1 && 2 && 3 || 4",
		"1 || 2 || 3 && 4"
	);
	INFO(exp);
	REQUIRE_THROWS(parse(exp));
}

TEST_CASE("Grouping", "[grammar]")
{
	REQUIRE_PARSE(
		"( 1 ) && ( 2 = 3 )",
		T<logical_and>(
			{T<leaf>("1"),
			 T<leaf_comparator>("=", {T<leaf>("2"), T<leaf>("3")})}
		)
	);

	REQUIRE_PARSE(
		"( 1 && 2 ) || ( 4 && 5 ) || 7 < 8",
		T<logical_or>(
			{T<logical_and>({T<leaf>("1"), T<leaf>("2")}),
			 T<logical_or>(
				 {T<logical_and>({T<leaf>("4"), T<leaf>("5")}),
				  T<leaf_comparator>("<", {T<leaf>("7"), T<leaf>("8")})}
			 )}
		)
	);
}

/**
 * Not expressions
 */
TEST_CASE("Not on leaf", "[grammar]")
{
	REQUIRE_PARSE("! a", T<logical_not>({T<leaf>("a")}));
}

TEST_CASE("Not on boolean expression", "[grammar]")
{
	REQUIRE_PARSE(
		"! a = b",
		T<logical_not>({T<leaf_comparator>("=", {T<leaf>("a"), T<leaf>("b")})})
	);
}

TEST_CASE("Not on condition", "[grammar]")
{
	REQUIRE_PARSE(
		"! ( a && b && c )",
		T<logical_not>({T<logical_and>(
			{T<leaf>("a"), T<logical_and>({T<leaf>("b"), T<leaf>("c")})}
		)})
	);
}

} // namespace ham::tests
