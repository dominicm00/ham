#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<statement_block>;

TEST_CASE("Statement blocks are non-empty", "[grammar]")
{
	auto stmt = GENERATE("", ";", " ;");
	REQUIRE_THROWS(parse(stmt));
}

TEST_CASE("Statements in block need semicolons", "[grammar]")
{
	auto stmt = GENERATE("Rule", "Rule a b", "Rule a b : c d;");
	REQUIRE_THROWS(parse(stmt));
}

TEST_CASE("Statements in block are separated by whitespace", "[grammar]")
{
	auto stmt = GENERATE("Rule B ;", "Rule a Echo c ;");
	REQUIRE_PARSE(stmt, T<statement_block>({T<rule_invocation>()}));
}

TEST_CASE("Statement block, 1 statement", "[grammar]")
{
	REQUIRE_PARSE(
		"Rule a ;",
		T<statement_block>({T<rule_invocation>("Rule a")})
	);
}

TEST_CASE("Statement block, 2 statements", "[grammar]")
{
	REQUIRE_PARSE(
		"Rule a ; Echo b ;",
		T<statement_block>(
			{T<rule_invocation>("Rule a"), T<rule_invocation>("Echo b")}
		)
	);
}

TEST_CASE("Statements, 3 statements", "[grammar]")
{
	REQUIRE_PARSE(
		"Rule a ; Echo b ; Do c : b ;",
		T<statement_block>(
			{T<rule_invocation>("Rule a"),
			 T<rule_invocation>("Echo b"),
			 T<rule_invocation>("Do c : b")}
		)
	);
}

} // namespace ham::tests