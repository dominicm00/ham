#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<condition>;

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

} // namespace ham::tests
