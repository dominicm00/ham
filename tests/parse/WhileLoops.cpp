#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/ParseUtils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<WhileLoop>;

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
		T<WhileLoop>(
			{T<Leaf>("a"),
			 T<StatementBlock>({T<RuleActionInvocation>({T<Identifier>("Echo")}
			 )})}
		)
	);
}

TEST_CASE("While loop with empty block", "[grammar]")
{
	REQUIRE_PARSE("while x { }", T<WhileLoop>({T<Leaf>("x"), T<EmptyBlock>()}));
}

} // namespace ham::tests
