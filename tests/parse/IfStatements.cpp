#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<IfStatement>;

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
		T<IfStatement>(
			{T<Leaf>("a"),
			 T<StatementBlock>({T<RuleActionInvocation>({T<Identifier>("Echo")}
			 )})}
		)
	);
}

TEST_CASE("If-else statement", "[grammar]")
{
	REQUIRE_PARSE(
		"if a { Echo ; } else { Rule ; }",
		T<IfStatement>(
			{T<Leaf>("a"),
			 T<StatementBlock>({T<RuleActionInvocation>({T<Identifier>("Echo")}
			 )}),
			 T<StatementBlock>({T<RuleActionInvocation>({T<Identifier>("Rule")}
			 )})}
		)
	);
}

TEST_CASE("If statements with empty blocks", "[grammar]")
{
	REQUIRE_PARSE(
		"if x { } else { }",
		T<IfStatement>({T<Leaf>("x"), T<EmptyBlock>(), T<EmptyBlock>()})
	);
	REQUIRE_PARSE(
		"if x { Rule ; } else { }",
		T<IfStatement>(
			{T<Leaf>("x"),
			 T<StatementBlock>({T<RuleActionInvocation>("Rule")}),
			 T<EmptyBlock>()}
		)
	);
	REQUIRE_PARSE(
		"if x { } else { Rule ; }",
		T<IfStatement>(
			{T<Leaf>("x"),
			 T<EmptyBlock>(),
			 T<StatementBlock>({T<RuleActionInvocation>("Rule")})}
		)
	);
}

} // namespace ham::tests
