#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<while_loop>;

TEST_CASE("While loop is non-empty", "[grammar]") { REQUIRE_THROWS(parse("")); }

TEST_CASE("While loop requires condition", "[grammar]")
{
	REQUIRE_THROWS(parse("while { }"));
}

TEST_CASE("While loop requires block", "[grammar]")
{
	REQUIRE_THROWS(parse("while a"));
	REQUIRE_THROWS(parse("while a "));
}

TEST_CASE("While loop", "[grammar]")
{
	REQUIRE_PARSE(
		"while a { Echo ; }",
		T<while_loop>(
			{T<leaf>("a"),
			 T<statement_block>({T<rule_invocation>({T<identifier>("Echo")})})}
		)
	);
}

TEST_CASE("While loop with empty block", "[grammar]")
{
	REQUIRE_PARSE(
		"while x { }",
		T<while_loop>({T<leaf>("x"), T<empty_block>()})
	);
}

} // namespace ham::tests
