#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<if_statement>;

TEST_CASE("If statement is non-empty", "[grammar]")
{
	REQUIRE_THROWS(parse(""));
}

TEST_CASE("If statement requires condition", "[grammar]")
{
	REQUIRE_THROWS(parse("if { }"));
}

TEST_CASE("If statement requires block", "[grammar]")
{
	REQUIRE_THROWS(parse("if a"));
	REQUIRE_THROWS(parse("if a "));
}

TEST_CASE("Else requires valid if statement", "[grammar]")
{
	auto stmt = GENERATE("else { }", "if { } else { }", "if a else { }");
	INFO(stmt);
	REQUIRE_THROWS(parse(stmt));
}

TEST_CASE("If statement", "[grammar]")
{
	REQUIRE_PARSE(
		"if a { Echo ; }",
		T<if_statement>(
			{T<leaf>("a"),
			 T<statement_block>({T<rule_invocation>({T<identifier>("Echo")})})}
		)
	);
}

TEST_CASE("If-else statement", "[grammar]")
{
	REQUIRE_PARSE(
		"if a { Echo ; } else { Rule ; }",
		T<if_statement>(
			{T<leaf>("a"),
			 T<statement_block>({T<rule_invocation>({T<identifier>("Echo")})}),
			 T<statement_block>({T<rule_invocation>({T<identifier>("Rule")})})}
		)
	);
}

TEST_CASE("If statements with empty blocks", "[grammar]")
{
	REQUIRE_PARSE(
		"if x { } else { }",
		T<if_statement>({T<leaf>("x"), T<empty_block>(), T<empty_block>()})
	);
	REQUIRE_PARSE(
		"if x { Rule ; } else { }",
		T<if_statement>(
			{T<leaf>("x"),
			 T<statement_block>({T<rule_invocation>("Rule")}),
			 T<empty_block>()}
		)
	);
	REQUIRE_PARSE(
		"if x { } else { Rule ; }",
		T<if_statement>(
			{T<leaf>("x"),
			 T<empty_block>(),
			 T<statement_block>({T<rule_invocation>("Rule")})}
		)
	);
}

} // namespace ham::tests
