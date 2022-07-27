#include "catch2/catch_test_macros.hpp"
#include "code/Grammar.hpp"
#include "tao/pegtl.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"
#include "tao/pegtl/rules.hpp"
#include "tao/pegtl/string_input.hpp"

namespace p = tao::pegtl;

template<typename Rule>
const auto genericParse = [](std::string str)
{
	auto input = p::string_input{str, "tests"};
	return p::parse<p::must<Rule, p::eof>>(input);
};

TEST_CASE("Identifiers", "[grammar]")
{
	const auto parse = genericParse<ham::code::Identifier>;

	SECTION("Alphanumeric identifiers are accepted")
	{
		REQUIRE_NOTHROW(parse("Id"));
		REQUIRE_NOTHROW(parse("Id1"));
		REQUIRE_NOTHROW(parse("1id2"));
		REQUIRE_NOTHROW(parse("UPPERCASE"));
		REQUIRE_NOTHROW(parse("CamelCase"));
		REQUIRE_NOTHROW(parse("veryLong14Id31IWith9Things"));
	}

	SECTION("Symbols in identifiers are not accepted")
	{
		REQUIRE_THROWS(parse("id-with-dash"));
		REQUIRE_THROWS(parse("id_underscore"));
		REQUIRE_THROWS(parse("other~!@#$%^&*()"));
		REQUIRE_THROWS(parse("{nobrack}"));
		REQUIRE_THROWS(parse("(noparen)"));
		REQUIRE_THROWS(parse("no\bslash"));
		REQUIRE_THROWS(parse("no/slash"));
		REQUIRE_THROWS(parse("\"noquote\""));
		REQUIRE_THROWS(parse("'nosquote'"));
	}
}
