#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<rule_definition>;

TEST_CASE("Rule definition is non-empty", "[grammar]")
{
	REQUIRE_THROWS(parse(""));
}

TEST_CASE("Rule definition, empty block", "[grammar]")
{
	auto rule = GENERATE("rule Rule { }", "rule Rule x : y { }");
	INFO(rule);
	REQUIRE(parse(rule));
}

TEST_CASE("Rule definition, non-empty block", "[grammar]")
{
	auto rule = GENERATE(
		"rule Rule { Echo x : y ; }",
		"rule Rule a : b : c { Echo x : y : z ; }"
	);
	INFO(rule);
	REQUIRE(parse(rule));
}

TEST_CASE("Rule definition, identifiers are required", "[grammar]")
{
	auto rule = GENERATE("rule { }", "rule Rule : { }");
	INFO(rule);
	REQUIRE_THROWS(parse(rule));
}

} // namespace ham::tests
