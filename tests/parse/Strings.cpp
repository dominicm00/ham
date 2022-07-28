#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
PARSE_FUNCTIONS(string)

/*
 * Tokens
 */
TEST_CASE("Identifiers are tokens", "[grammar]")
{
	auto str = GENERATE(
		"Id",
		"Id1",
		"1id2",
		"UPPERCASE",
		"CamelCase",
		"veryLong14Id31IWith9Things"
	);
	REQUIRE(identity(str));
}

TEST_CASE("Symbols are allowed in tokens", "[grammar]")
{
	auto str = GENERATE(
		"id-with-dash",
		"id_underscore",
		"other~!@#$%^&*()",
		"{brack}",
		"(paren)",
		"\bslash",
		"/slash",
		"<grist>a/relative/path.cpp(member)"
	);
	REQUIRE(identity(str));
}

TEST_CASE("Unquoted whitespace is not allowed in tokens", "[grammar]")
{
	auto str = GENERATE(
		"id with space",
		"id        many spaces",
		"id\t\ttabs",
		"id\nnewline",
		"id \t mixed \n up \f whitespace"
	);
	REQUIRE_FALSE(parse(str));
}

TEST_CASE("Escape sequences are literals in tokens", "[grammar]")
{
	auto str = GENERATE("my\\newline", "\\t\\f\\a\\b", "\\\"\\\"", "\\'\\'");
	REQUIRE(identity(str));
}

/*
 * Quoted strings
 */
TEST_CASE("Whitespace is allowed in quoted strings", "grammar")
{
	REQUIRE(
		content("'lots of \t whitespace \n newline'")
		== "lots of \t whitespace \n newline"
	);
	REQUIRE(
		content("\"lots of \t whitespace \n newline\"")
		== "lots of \t whitespace \n newline"
	);
}

TEST_CASE("Escape sequences are literals in single quoted string", "[grammar]")
{
	REQUIRE(content("'my\\newline'") == "my\\newline");
	REQUIRE(content("'\\t\\f\\a\\b'") == "\\t\\f\\a\\b");
}

TEST_CASE("Escape sequences are parsed in double quoted string", "[grammar]")
{
	REQUIRE(content("\"my\\newline\"") == "my\newline");
	REQUIRE(content("\"\\t\\f\\a\\b\"") == "\t\f\a\b");
}

TEST_CASE("Quotes can be escaped in quoted strings", "[grammar]")
{
	REQUIRE(
		content("'I\'m a single \\\"quoted\\\" string'")
		== "I'm a single \"quoted\" string"
	);
	REQUIRE(
		content("\"I\'m a double \\\"quoted\\\" string\"")
		== "I'm a double \"quoted\" string"
	);
}

TEST_CASE("Single quotes can be unescaped in double quotes", "[grammar]")
{
	REQUIRE(content("\"'This' is fin'e\"") == "'This' is fin'e");
}

TEST_CASE("Double quotes can be unescaped in single quotes", "[grammar]")
{
	REQUIRE(content("'\"This\" is fin\"e'") == "\"This\" is fin\"e");
}
} // namespace ham::tests
