#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<Identifier>;

TEST_CASE("Alphanumeric identifiers", "[grammar]")
{
	auto id = GENERATE(
		"Id",
		"Id1",
		"1id2",
		"UPPERCASE",
		"CamelCase",
		"veryLong14Id31IWith9Things"
	);
	REQUIRE(parse(id));
}

TEST_CASE("Valid symbols in identifiers", "[grammar]")
{
	auto id = GENERATE("_", "_hi", "test_", "-dash-", "/", "\\");
	REQUIRE(parse(id));
}

TEST_CASE("Invalid symbols in identifiers", "[grammar]")
{
	auto id = GENERATE(
		"!",
		"@",
		"#",
		"%",
		"^",
		"&",
		"*",
		"|",
		":",
		";",
		"'",
		"\"",
		"{}",
		"()",
		"[]",
		"<>"
	);
	REQUIRE_THROWS(parse(id));
}

TEST_CASE("Invalid whitespace in identifiers", "[grammar]")
{
	auto id = GENERATE(
		"id with space",
		"id        many spaces",
		"id\t\ttabs",
		"id\nnewline",
		"id \t mixed \n up \f whitespace"
	);
	REQUIRE_THROWS(parse(id));
}

TEST_CASE("Variables in identifiers", "[grammar]")
{
	REQUIRE_PARSE(
		"a$(b)",
		T<Identifier>({T<IdChar>("a"), T<Variable>("$(b)")})
	);
}

} // namespace ham::tests
