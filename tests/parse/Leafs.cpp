#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "parse/PegtlParser.hpp"
#include "tao/pegtl/string_input.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<Leaf>;

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
		"<grist>a/relative/path.cpp",
		"@",
		"%",
		"^",
		"*"
	);
	INFO(str);
	REQUIRE(parse(str));
}

TEST_CASE("Reserved symbols are not allowed in words", "[grammar]")
{
	auto str = GENERATE("$", "'", "\"", ":", ";", "|", "{brack}", "(paren)");
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
	REQUIRE_PARSE("'$n$t$\"'", T<Leaf>({T<QuotedSingleContent>("$n$t$\"")}));
}

TEST_CASE("Escape sequences are parsed in double quoted string", "[grammar]")
{
	REQUIRE_PARSE(
		"\"$n$tx$a$b\"",
		T<Leaf>({T<QuotedDouble>(
			{T<SpecialEscape>("n"),
			 T<SpecialEscape>("t"),
			 T<QuotedChar>("x"),
			 T<SpecialEscape>("a"),
			 T<SpecialEscape>("b")}
		)})
	);
}

TEST_CASE("Quotes can be escaped in double quoted strings", "[grammar]")
{
	REQUIRE_PARSE(
		"\"$'$\"\"",
		T<Leaf>({T<QuotedDouble>({T<CharEscape>("'"), T<CharEscape>("\"")})})
	);
}

TEST_CASE("Quotes cannot be escaped in single quoted strings", "[grammar]")
{
	REQUIRE_THROWS(parse("'$''"));
}

TEST_CASE("Single quotes can be unescaped in double quotes", "[grammar]")
{
	REQUIRE_PARSE(
		"\"a'b\"",
		T<Leaf>({T<QuotedDouble>(
			{T<QuotedChar>("a"), T<QuotedChar>("'"), T<QuotedChar>("b")}
		)})
	);
}

TEST_CASE("Double quotes can be unescaped in single quotes", "[grammar]")
{
	REQUIRE_PARSE("'a\"b'", T<Leaf>({T<QuotedSingleContent>("a\"b")}));
}

TEST_CASE("Escape sequences must be valid", "[grammar]")
{
	std::string escape = GENERATE(":", ";", "\\", "|");
	INFO(escape);
	std::string str = "\"$" + escape + "\"";
	REQUIRE_THROWS(parse(str));
}

/**
 * Bracket expressions
 */
TEST_CASE("Bracket expressions recognized as leafs", "[grammar]")
{
	REQUIRE_PARSE("[ Rule ]", T<Leaf>({T<RuleActionInvocation>("Rule")}));
	REQUIRE_PARSE(
		"[ on target Rule ]",
		T<Leaf>({T<TargetRuleInvocation>(
			{T<Leaf>("target"), T<RuleActionInvocation>("Rule")}
		)})
	);
}

TEST_CASE("Embedded bracket expressions ignored", "[grammar]")
{
	REQUIRE_THROWS(parse("a[ B ]"));
	REQUIRE_PARSE(
		"\"a[ B ]\"",
		T<Leaf>({T<QuotedDouble>(
			{T<QuotedChar>("a"),
			 T<QuotedChar>("["),
			 T<QuotedChar>(" "),
			 T<QuotedChar>("B"),
			 T<QuotedChar>(" "),
			 T<QuotedChar>("]")}
		)})
	);
}

/**
 * Complex leafs
 */
TEST_CASE("Different types of leafs can be combined", "[grammar]")
{
	REQUIRE_PARSE(
		"a'b c'd\"$n\"",
		T<Leaf>(
			{T<Word>("a"),
			 T<QuotedSingleContent>("b c"),
			 T<Word>("d"),
			 T<QuotedDouble>({T<SpecialEscape>("n")})}
		)
	);
}

TEST_CASE("Complex leafs reject Word whitespace", "[grammar]")
{
	auto str = GENERATE("a'b c'd e", "\"a b c\" d");
	REQUIRE_THROWS(parse(str));
}

TEST_CASE("Words nest variables", "[grammar]")
{
	REQUIRE_PARSE(
		"a$(b)",
		T<Leaf>({T<Word>("a"), T<Variable>({T<Identifier>("b")})})
	);
}

TEST_CASE("Double quotes nest variables", "[grammar]")
{
	REQUIRE_PARSE(
		"\"a$(b)\"",
		T<Leaf>({T<QuotedDouble>(
			{T<QuotedChar>("a"), T<Variable>({T<Identifier>("b")})}
		)})
	);
}

TEST_CASE("Single quotes don't nest variables", "[grammar]")
{
	REQUIRE_PARSE("'a$(b)'", T<Leaf>({T<QuotedSingleContent>("a$(b)")}));
}

} // namespace ham::tests
