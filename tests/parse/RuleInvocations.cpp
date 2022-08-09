#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<rule_invocation>;

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
	REQUIRE_PARSE(rule, T<rule_invocation>({T<identifier>(rule)}));
}

TEST_CASE("Rule invocation, 1 argument", "[grammar]")
{
	REQUIRE_PARSE(
		"Rule a",
		T<rule_invocation>({T<identifier>("Rule"), T<list>({T<leaf>("a")})})
	);
	REQUIRE_PARSE(
		"Rule a b c",
		T<rule_invocation>(
			{T<identifier>("Rule"),
			 T<list>({T<leaf>("a"), T<leaf>("b"), T<leaf>("c")})}
		)
	);
}

TEST_CASE("Rule invocation, 2 arguments", "[grammar]")
{
	REQUIRE_PARSE(
		"Rule a : b",
		T<rule_invocation>(
			{T<identifier>("Rule"),
			 T<list>({T<leaf>("a")}),
			 T<rule_separator>(),
			 T<list>({T<leaf>("b")})}
		)
	);
	REQUIRE_PARSE(
		"Rule a b c : d e f",
		T<rule_invocation>(
			{T<identifier>("Rule"),
			 T<list>({T<leaf>("a"), T<leaf>("b"), T<leaf>("c")}),
			 T<rule_separator>(),
			 T<list>({T<leaf>("d"), T<leaf>("e"), T<leaf>("f")})}
		)
	);
}

TEST_CASE("Rule invocation, empty arguments", "[grammar]")
{
	REQUIRE_PARSE(
		"Rule : b",
		T<rule_invocation>(
			{T<identifier>("Rule"),
			 T<rule_separator>(),
			 T<list>({T<leaf>("b")})}
		)
	);
	REQUIRE_PARSE(
		"Rule a :",
		T<rule_invocation>(
			{T<identifier>("Rule"),
			 T<list>({T<leaf>("a")}),
			 T<rule_separator>()}
		)
	);
	REQUIRE_PARSE(
		"Rule :",
		T<rule_invocation>({T<identifier>("Rule"), T<rule_separator>()})
	);
}

} // namespace ham::tests
