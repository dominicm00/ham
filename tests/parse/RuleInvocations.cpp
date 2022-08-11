#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<RuleInvocation>;

TEST_CASE("Rule invocation is non-empty", "[grammar]")
{
	REQUIRE_THROWS(parse(""));
}

TEST_CASE("Rule invocation doesn't consume trailing whitespace", "[grammar]")
{
	auto rule = GENERATE("Rule ", "Rule a ", "Rule a : b ", "Rule : ");
	INFO(rule);
	REQUIRE_THROWS(parse(rule));
}

TEST_CASE("Rule invocation, no argument", "[grammar]")
{
	std::string rule = GENERATE("Rule", "dyn$(amic)", "with-symbols/");
	INFO(rule);
	REQUIRE_PARSE(rule, T<RuleInvocation>({T<Identifier>(rule)}));
}

TEST_CASE("Rule invocation, 1 argument", "[grammar]")
{
	REQUIRE_PARSE(
		"Rule a",
		T<RuleInvocation>({T<Identifier>("Rule"), T<List>({T<Leaf>("a")})})
	);
	REQUIRE_PARSE(
		"Rule a b c",
		T<RuleInvocation>(
			{T<Identifier>("Rule"),
			 T<List>({T<Leaf>("a"), T<Leaf>("b"), T<Leaf>("c")})}
		)
	);
}

TEST_CASE("Rule invocation, 2 arguments", "[grammar]")
{
	REQUIRE_PARSE(
		"Rule a : b",
		T<RuleInvocation>(
			{T<Identifier>("Rule"),
			 T<List>({T<Leaf>("a")}),
			 T<RuleSeparator>(),
			 T<List>({T<Leaf>("b")})}
		)
	);
	REQUIRE_PARSE(
		"Rule a b c : d e f",
		T<RuleInvocation>(
			{T<Identifier>("Rule"),
			 T<List>({T<Leaf>("a"), T<Leaf>("b"), T<Leaf>("c")}),
			 T<RuleSeparator>(),
			 T<List>({T<Leaf>("d"), T<Leaf>("e"), T<Leaf>("f")})}
		)
	);
}

TEST_CASE("Rule invocation, empty arguments", "[grammar]")
{
	REQUIRE_PARSE(
		"Rule : b",
		T<RuleInvocation>(
			{T<Identifier>("Rule"), T<RuleSeparator>(), T<List>({T<Leaf>("b")})}
		)
	);
	REQUIRE_PARSE(
		"Rule a :",
		T<RuleInvocation>(
			{T<Identifier>("Rule"), T<List>({T<Leaf>("a")}), T<RuleSeparator>()}
		)
	);
	REQUIRE_PARSE(
		"Rule :",
		T<RuleInvocation>({T<Identifier>("Rule"), T<RuleSeparator>()})
	);
}

} // namespace ham::tests
