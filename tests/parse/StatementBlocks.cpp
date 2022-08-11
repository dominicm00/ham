#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tao/pegtl/contrib/trace.hpp"
#include "tao/pegtl/string_input.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<StatementBlock>;

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
	REQUIRE_PARSE(stmt, T<StatementBlock>({T<RuleInvocation>()}));
}

TEST_CASE("Statement block, 1 statement", "[grammar]")
{
	REQUIRE_PARSE("Rule a ;", T<StatementBlock>({T<RuleInvocation>("Rule a")}));
}

TEST_CASE("Statement block, 2 statements", "[grammar]")
{
	REQUIRE_PARSE(
		"Rule a ; Echo b ;",
		T<StatementBlock>(
			{T<RuleInvocation>("Rule a"), T<RuleInvocation>("Echo b")}
		)
	);
}

TEST_CASE("Statement block, 3 statements", "[grammar]")
{
	REQUIRE_PARSE(
		"Rule a ; Echo b ; Do c : b ;",
		T<StatementBlock>(
			{T<RuleInvocation>("Rule a"),
			 T<RuleInvocation>("Echo b"),
			 T<RuleInvocation>("Do c : b")}
		)
	);
}

TEST_CASE("Statement block, definitions", "[grammar]")
{
	REQUIRE_PARSE(
		"actions Action { } Echo x ; rule Rule { }",
		T<StatementBlock>(
			{T<ActionDefinition>(), T<RuleInvocation>(), T<RuleDefinition>()}
		)
	);
}

TEST_CASE("Statement block, if statement", "[grammar]")
{
	REQUIRE_PARSE(
		"Echo x ; if x { } else { } Echo again ;",
		T<StatementBlock>(
			{T<RuleInvocation>("Echo x"),
			 T<IfStatement>({T<Leaf>("x"), T<EmptyBlock>(), T<EmptyBlock>()}),
			 T<RuleInvocation>("Echo again")}
		)
	);
}

TEST_CASE("Statement block, while loop", "[grammar]")
{
	REQUIRE_PARSE(
		"Echo x ; while x { } Echo again ;",
		T<StatementBlock>(
			{T<RuleInvocation>("Echo x"),
			 T<WhileLoop>({T<Leaf>("x"), T<EmptyBlock>()}),
			 T<RuleInvocation>("Echo again")}
		)
	);
}

TEST_CASE("Statement block, for loop", "[grammar]")
{
	REQUIRE_PARSE(
		"Echo x ; for x in $(a) { } Echo again ;",
		T<StatementBlock>(
			{T<RuleInvocation>("Echo x"),
			 T<ForLoop>(
				 {T<Identifier>("x"),
				  T<Leaf>({T<Variable>("$(a)")}),
				  T<EmptyBlock>()}
			 ),
			 T<RuleInvocation>("Echo again")}
		)
	);
}

TEST_CASE("Statement block, target statement", "[grammar]")
{
	REQUIRE_PARSE(
		"Echo x ; on target Echo x ; Echo again ;",
		T<StatementBlock>(
			{T<RuleInvocation>("Echo x"),
			 T<TargetStatement>({T<Leaf>("target"), T<RuleInvocation>("Echo x")}
			 ),
			 T<RuleInvocation>("Echo again")}
		)
	);
}

} // namespace ham::tests
