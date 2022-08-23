#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/ParseUtils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<List>;

TEST_CASE("Lists cannot be empty", "[grammar]")
{
	// Applications of lists may allow them to be empty, but the caller has to
	// handle that case based on separators.
	auto lst = GENERATE("", " ", "  ");
	REQUIRE_THROWS(parse(lst));
}

TEST_CASE("Lists do not consume surrounding whitespace", "[grammar]")
{
	auto lst = GENERATE(" a ", " a", "a ");
	REQUIRE_THROWS(parse(lst));
}

TEST_CASE("Lists, size 1", "[grammar]")
{
	auto lst = GENERATE(
		"word1",
		"'quoted string'",
		"\"conca ten\"ated",
		"lots'of quoted white'\"space present\""
	);
	REQUIRE_PARSE(lst, T<List>({T<Leaf>()}));
}

TEST_CASE("Lists, size 2", "[grammar]")
{
	auto lst = GENERATE(
		"word1 word2",
		"'quoted string' word",
		"\"concaten\"ated forms",
		"lots'of quoted white'\"space present\" word"
	);
	REQUIRE_PARSE(lst, T<List>({T<Leaf>(), T<Leaf>()}));
}

TEST_CASE("Lists, size 3", "[grammar]")
{
	auto lst = GENERATE(
		"word1 word2 word3",
		"'quoted string' word \"again\"",
		"\"concaten\"ated forms twice",
		"'something' lots'of quoted white'\"space present\" word"
	);
	REQUIRE_PARSE(lst, T<List>({T<Leaf>(), T<Leaf>(), T<Leaf>()}));
}

} // namespace ham::tests
