#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<ForLoop>;

TEST_CASE("For loop is non-empty", "[grammar]") { REQUIRE_THROWS(parse("")); }

TEST_CASE("For loop requires identifier", "[grammar]")
{
	auto loop = GENERATE(
		"for symbol! in $(x) { }",
		"for in $(x) { }",
		"for 'abc' in $(x) { }"
	);
	INFO(loop);
	REQUIRE_THROWS(parse(loop));
}

TEST_CASE("For loop requires leaf", "[grammar]")
{
	auto loop = GENERATE(
		"for x in a = b { }",
		"for x in 1 2 3 { }",
		"for x in Echo y { }"
	);
	INFO(loop);
	REQUIRE_THROWS(parse(loop));
}

TEST_CASE("For loop requires 'in'", "[grammar]")
{
	auto loop = GENERATE("for x a { }", "for x $(y) { }");
	INFO(loop);
	REQUIRE_THROWS(parse(loop));
}

TEST_CASE("For loop requires block", "[grammar]")
{
	auto loop = GENERATE("for x in x", "for x in x ");
	INFO(loop);
	REQUIRE_THROWS(parse(loop));
}

TEST_CASE("For loop", "[grammar]")
{
	REQUIRE_PARSE(
		"for x in $(y) { Echo ; }",
		T<ForLoop>(
			{T<Identifier>("x"),
			 T<Leaf>({T<Variable>({T<Identifier>("y")})}),
			 T<StatementBlock>({T<RuleActionInvocation>({T<Identifier>("Echo")}
			 )})}
		)
	);
}

TEST_CASE("For loop with empty block", "[grammar]")
{
	REQUIRE_PARSE(
		"for x in $(y) { }",
		T<ForLoop>(
			{T<Identifier>("x"),
			 T<Leaf>({T<Variable>({T<Identifier>("y")})}),
			 T<EmptyBlock>()}
		)
	);
}

} // namespace ham::tests
