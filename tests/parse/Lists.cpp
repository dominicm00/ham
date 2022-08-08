#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<list>;

TEST_CASE("Lists are non-empty", "[grammar]") { REQUIRE_FALSE(parse("")); }

TEST_CASE("Lists, size 1", "[grammar]")
{
	auto id = GENERATE(
		"word1",
		"'quoted string'",
		"\"conca ten\"ated",
		"lots'of quoted white'\"space present\""
	);
	REQUIRE_PARSE(id, T<list>({T<leaf>()}));
}

TEST_CASE("Lists, size 2", "[grammar]")
{
	auto id = GENERATE(
		"word1 word2",
		"'quoted string' word",
		"\"concaten\"ated forms",
		"lots'of quoted white'\"space present\" word"
	);
	REQUIRE_PARSE(id, T<list>({T<leaf>(), T<leaf>()}));
}

TEST_CASE("Lists, size 3", "[grammar]")
{
	auto id = GENERATE(
		"word1 word2 word3",
		"'quoted string' word \"again\"",
		"\"concaten\"ated forms twice",
		"'something' lots'of quoted white'\"space present\" word"
	);
	REQUIRE_PARSE(id, T<list>({T<leaf>(), T<leaf>(), T<leaf>()}));
}

} // namespace ham::tests
