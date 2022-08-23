#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/ParseUtils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<RuleSignature>;

TEST_CASE("Rule signature is non-empty", "[grammar]")
{
	REQUIRE_THROWS(parse(""));
}

TEST_CASE("Rule signature does not consume trailing whitespace", "[grammar]")
{
	auto sig = GENERATE("rule Rule ", "rule Rule x ", "rule Rule x : y ");
	INFO(sig);
	REQUIRE_THROWS(parse(sig));
}

TEST_CASE("Rule signature, identifiers", "[grammar]")
{
	REQUIRE_PARSE("rule Rule", T<RuleSignature>({T<Identifier>("Rule")}));
	REQUIRE_PARSE(
		"rule Rule x",
		T<RuleSignature>({T<Identifier>("Rule"), T<Identifier>("x")})
	);
	REQUIRE_PARSE(
		"rule Rule x : y",
		T<RuleSignature>(
			{T<Identifier>("Rule"), T<Identifier>("x"), T<Identifier>("y")}
		)
	);
	REQUIRE_PARSE(
		"rule Rule x : y : z",
		T<RuleSignature>(
			{T<Identifier>("Rule"),
			 T<Identifier>("x"),
			 T<Identifier>("y"),
			 T<Identifier>("z")}
		)
	);
}

TEST_CASE("Rule signature, identifiers are required", "[grammar]")
{
	REQUIRE_THROWS(parse("rule"));
}

} // namespace ham::tests
