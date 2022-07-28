#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
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

using namespace ham::parse;

TEST_CASE("code/Words: Identifiers", "[grammar]")
{
	const auto parse = genericParse<identifier>;

	SECTION("Alphanumeric identifiers are accepted")
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

	SECTION("Symbols in identifiers are not accepted")
	{
		auto id = GENERATE(
			"id-with-dash",
			"id_underscore",
			"other~!@#$%^&*()",
			"{nobrack}",
			"(noparen)",
			"no\bslash",
			"no/slash",
			"\"noquote\"",
			"'nosquote'"
		);
		REQUIRE_FALSE(parse(id));
	}

	SECTION("Whitespace in identifiers is not accepted")
	{
		auto id = GENERATE(
			"id with space",
			"id        many spaces",
			"id\t\ttabs",
			"id\nnewline",
			"id \t mixed \n up \f whitespace"
		);
		REQUIRE_FALSE(parse(id));
	}
}

TEST_CASE("code/Words: Simple variables", "[grammar]")
{
	const auto parse = genericParse<variable>;

	SECTION("Identifiers are not accepted")
	{
		auto id = GENERATE("Id", "Id1", "1id2", "UPPERCASE", "CamelCase");
		REQUIRE_FALSE(parse(id));
	}

	SECTION("Variable must be surrounded by '$()'")
	{
		auto id = GENERATE(
			"not$(surrounded)",
			"$(missingend",
			"missingbegin)",
			"(missingdollar)",
			"$(bad)suffix"
		);
		REQUIRE_FALSE(parse(id));
	}

	SECTION("Whitespace is accepted between '$(  )'")
	{
		auto id = GENERATE("$( Whitespace )", "$(UnevenF )", "$( UnevenB)");
		REQUIRE(parse(id));
	}

	SECTION("Identifier is only child")
	{
		std::string str = "$(Variable)";
		auto var = decompose(parse(str), {0});
		check(var, T<variable>({T<identifier>("Variable")}));
	}
}

TEST_CASE("code/Words: Subscripts", "[grammar]")
{
	const auto parse = genericParse<variable>;

	SECTION("Single element subscripts")
	{
		std::string str = "$(var[3])";
		auto var = decompose(parse(str), {0});
		check(
			var,
			T<variable>({T<identifier>("var"), T<subscript>({T<number>("3")})})
		);
	}

	SECTION("Start-only range subscripts")
	{
		std::string str = "$(var[3-])";
		auto var = decompose(parse(str), {0});
		check(
			var,
			T<variable>(
				{T<identifier>("var"),
				 T<subscript>({T<number>("3"), T<end_subscript>()})}
			)
		);
	}

	SECTION("Range subscripts")
	{
		std::string str = "$(var[3-5])";
		auto var = decompose(parse(str), {0});
		check(
			var,
			T<variable>(
				{T<identifier>("var"),
				 T<subscript>({T<number>("3"), T<number>("5")})}
			)
		);
	}
}

} // namespace ham::tests
