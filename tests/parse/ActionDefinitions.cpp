#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<ActionDefinition>;

TEST_CASE("Action definition is non-empty", "[grammar]")
{
	REQUIRE_THROWS(parse(""));
}

TEST_CASE("Action definition, empty block", "[grammar]")
{
	REQUIRE(parse("action Action { }"));
}

TEST_CASE("Action definition accepts action/actions as keyword", "[grammar]")
{
	auto rule = GENERATE("action Action { }", "actions Action { }");
	INFO(rule);
	REQUIRE(parse(rule));
}

TEST_CASE("Action definition, identifier is required", "[grammar]")
{
	auto rule = GENERATE("action { }", "actions { }");
	INFO(rule);
	REQUIRE_THROWS(parse(rule));
}

TEST_CASE("Action definition, simple content", "[grammar]")
{
	REQUIRE_PARSE(
		"action Action { echo x }",
		T<ActionDefinition>(
			{T<Identifier>("Action"), T<ActionString>("echo x ")}
		)
	);
}

TEST_CASE("Action definition, variable", "[grammar]")
{
	REQUIRE_PARSE(
		"action Action { echo $(y) }",
		T<ActionDefinition>(
			{T<Identifier>("Action"),
			 T<ActionString>("echo "),
			 T<Variable>("$(y)"),
			 T<ActionString>(" ")}
		)
	);
}

TEST_CASE("Action definition, escape sequences", "[grammar]")
{
	REQUIRE_PARSE(
		"action Action { $$$}test }",
		T<ActionDefinition>(
			{T<Identifier>("Action"),
			 T<ActionEscape>("$"),
			 T<ActionEscape>("}"),
			 T<ActionString>("test ")}
		)
	);
}

} // namespace ham::tests
