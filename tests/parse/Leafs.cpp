#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<leaf>;

/*
 * Words
 */
TEST_CASE("Identifiers are words", "[grammar]")
{
	auto str = GENERATE(
		"Id",
		"Id1",
		"1id2",
		"UPPERCASE",
		"CamelCase",
		"veryLong14Id31IWith9Things"
	);
	INFO(str);
	REQUIRE(parse(str));
}

TEST_CASE("Symbols are allowed in words", "[grammar]")
{
	auto str = GENERATE(
		"id-with-dash",
		"id_underscore",
		"\\bslash",
		"/slash",
		"<grist>a/relative/path.cpp(member)",
		"(paren)",
		"b{rack}",
		"[squ]are",
		"!",
		"@",
		"#",
		"%",
		"^",
		"&",
		"*"
	);
	INFO(str);
	REQUIRE(parse(str));
}

TEST_CASE("Reserved symbols are not allowed in words", "[grammar]")
{
	auto str = GENERATE("$", "'", "\"", ":", ";", "|");
	INFO(str);
	REQUIRE_THROWS(parse(str));
}

TEST_CASE("Unclosed variables are not allowed", "[grammar]")
{
	auto str = GENERATE("$", "$(", "$(var[2-3]");
	INFO(str);
	REQUIRE_THROWS(parse(str));
}

TEST_CASE("Unquoted whitespace is not allowed in words", "[grammar]")
{
	auto str = GENERATE(
		"id with space",
		"id        many spaces",
		"id\t\ttabs",
		"id\nnewline",
		"id \t mixed \n up \f whitespace"
	);
	INFO(str);
	REQUIRE_THROWS(parse(str));
}

TEST_CASE("Escape sequences are literalss in words", "[grammar]")
{
	REQUIRE_PARSE(
		"\\n\\t",
		T<leaf>(
			{T<string_char>("\\"),
			 T<string_char>("n"),
			 T<string_char>("\\"),
			 T<string_char>("t")}
		)
	);
}

TEST_CASE("Strings cannot be empty", "[grammar]") { REQUIRE_THROWS(parse("")); }

/*
 * Quoted strings
 */
TEST_CASE("Whitespace is allowed in quoted strings", "[grammar]")
{
	REQUIRE(parse("'lots of \t whitespace \n newline'"));
	REQUIRE(parse("\"lots of \t whitespace \n newline\""));
}

TEST_CASE("Escape sequences are literals in single quoted string", "[grammar]")
{
	REQUIRE_PARSE(
		"'\\n\\t'",
		T<leaf>(
			{T<string_char>("\\"),
			 T<string_char>("n"),
			 T<string_char>("\\"),
			 T<string_char>("t")}
		)
	);
}

TEST_CASE("Escape sequences are parsed in double quoted string", "[grammar]")
{
	REQUIRE_PARSE(
		"\"\\n\\tx\\a\\b\"",
		T<leaf>(
			{T<special_escape>("n"),
			 T<special_escape>("t"),
			 T<string_char>("x"),
			 T<special_escape>("a"),
			 T<special_escape>("b")}
		)
	);
}

TEST_CASE("Quotes can be escaped in quoted strings", "[grammar]")
{
	REQUIRE_PARSE(
		"'\\'\\\"'",
		T<leaf>({T<char_escape>("'"), T<char_escape>("\"")})
	);
	REQUIRE_PARSE(
		"\"\\'\\\"\"",
		T<leaf>({T<char_escape>("'"), T<char_escape>("\"")})
	);
}

TEST_CASE("Single quotes can be unescaped in double quotes", "[grammar]")
{
	REQUIRE_PARSE(
		"\"a'b\"",
		T<leaf>({T<string_char>("a"), T<string_char>("'"), T<string_char>("b")})
	);
}

TEST_CASE("Double quotes can be unescaped in single quotes", "[grammar]")
{
	REQUIRE_PARSE(
		"'a\"b'",
		T<leaf>({T<string_char>("a"), T<string_char>("\""), T<string_char>("b")}
		)
	);
}

/**
 * Complex leafs
 */
TEST_CASE("Different types of leafs can be combined", "[grammar]")
{
	REQUIRE_PARSE(
		"a'b c'd\"\\n\"",
		T<leaf>(
			{T<string_char>("a"),
			 T<string_char>("b"),
			 T<string_char>(" "),
			 T<string_char>("c"),
			 T<string_char>("d"),
			 T<special_escape>("n")}
		)
	);
}

TEST_CASE("Complex leafs reject word whitespace", "[grammar]")
{
	auto str = GENERATE("a'b c'd e", "\"a b c\" d");
	REQUIRE_THROWS(parse(str));
}

TEST_CASE("Words nest variables", "[grammar]")
{
	REQUIRE_PARSE(
		"a$(b)",
		T<leaf>({T<string_char>("a"), T<variable>({T<identifier>("b")})})
	);
}

TEST_CASE("Double quotes nest variables", "[grammar]")
{
	REQUIRE_PARSE(
		"\"a$(b)\"",
		T<leaf>({T<string_char>("a"), T<variable>({T<identifier>("b")})})
	);
}

TEST_CASE("Single quotes don't nest variables", "[grammar]")
{
	REQUIRE_PARSE(
		"'a$(b)'",
		T<leaf>(
			{T<string_char>("a"),
			 T<string_char>("$"),
			 T<string_char>("("),
			 T<string_char>("b"),
			 T<string_char>(")")}
		)
	);
}
} // namespace ham::tests
