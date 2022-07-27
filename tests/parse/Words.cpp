#include "catch2/catch_test_macros.hpp"
#include "parse/Grammar.hpp"
#include "tao/pegtl.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"
#include "tao/pegtl/demangle.hpp"
#include "tao/pegtl/memory_input.hpp"
#include "tao/pegtl/rules.hpp"
#include "tao/pegtl/string_input.hpp"
#include "tests/Utils.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace ham::tests
{

using namespace ham::code;

TEST_CASE("code/Words: Identifiers", "[grammar]")
{
	const auto parse = genericParse<Identifier>;

	SECTION("Alphanumeric identifiers are accepted")
	{
		REQUIRE(parse("Id"));
		REQUIRE(parse("Id1"));
		REQUIRE(parse("1id2"));
		REQUIRE(parse("UPPERCASE"));
		REQUIRE(parse("CamelCase"));
		REQUIRE(parse("veryLong14Id31IWith9Things"));
	}

	SECTION("Symbols in identifiers are not accepted")
	{
		REQUIRE_FALSE(parse("id-with-dash"));
		REQUIRE_FALSE(parse("id_underscore"));
		REQUIRE_FALSE(parse("other~!@#$%^&*()"));
		REQUIRE_FALSE(parse("{nobrack}"));
		REQUIRE_FALSE(parse("(noparen)"));
		REQUIRE_FALSE(parse("no\bslash"));
		REQUIRE_FALSE(parse("no/slash"));
		REQUIRE_FALSE(parse("\"noquote\""));
		REQUIRE_FALSE(parse("'nosquote'"));
	}

	SECTION("Whitespace in identifiers is not accepted")
	{
		REQUIRE_FALSE(parse("id with space"));
		REQUIRE_FALSE(parse("id        many spaces"));
		REQUIRE_FALSE(parse("id\t\ttabs"));
		REQUIRE_FALSE(parse("id\nnewline"));
		REQUIRE_FALSE(parse("id \t mixed \n up \f whitespace"));
	}
}

TEST_CASE("code/Words: Simple variables", "[grammar]")
{
	const auto parse = genericParse<Variable>;

	SECTION("Identifiers are not accepted")
	{
		REQUIRE_FALSE(parse("Id"));
		REQUIRE_FALSE(parse("Id1"));
		REQUIRE_FALSE(parse("1id2"));
		REQUIRE_FALSE(parse("UPPERCASE"));
		REQUIRE_FALSE(parse("CamelCase"));
		REQUIRE_FALSE(parse("veryLong14Id31IWith9Things"));
	}

	SECTION("Variable must be surrounded by '$()'")
	{
		REQUIRE_FALSE(parse("not$(surrounded)"));
		REQUIRE_FALSE(parse("$(missingend"));
		REQUIRE_FALSE(parse("missingbegin)"));
		REQUIRE_FALSE(parse("(missingdollar)"));
		REQUIRE_FALSE(parse("$(bad)suffix"));
	}

	SECTION("Whitespace is accepted between '$(  )'")
	{
		REQUIRE("$( Whitespace )");
		REQUIRE("$(Uneven )");
	}

	SECTION("Identifier is only child")
	{
		std::string str = "$(Variable)";
		auto var = decompose(parse(str), {0});
		check(var, T<Variable>({T<Identifier>("Variable")}));
	}
}

TEST_CASE("code/Words: Subscripts", "[grammar]")
{
	const auto parse = genericParse<ham::code::Variable>;

	SECTION("Single element subscripts")
	{
		std::string str = "$(var[3])";
		auto var = decompose(parse(str), {0});
		check(
			var,
			T<Variable>({T<Identifier>("var"), T<Subscript>({T<Number>("3")})})
		);
	}

	SECTION("Start-only range subscripts")
	{
		std::string str = "$(var[3-])";
		auto var = decompose(parse(str), {0});
		check(
			var,
			T<Variable>(
				{T<Identifier>("var"),
				 T<Subscript>({T<Number>("3"), T<EndSubscript>()})}
			)
		);
	}

	SECTION("Range subscripts")
	{
		std::string str = "$(var[3-5])";
		auto var = decompose(parse(str), {0});
		check(
			var,
			T<Variable>(
				{T<Identifier>("var"),
				 T<Subscript>({T<Number>("3"), T<Number>("5")})}
			)
		);
	}
}

} // namespace ham::tests
