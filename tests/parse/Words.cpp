#include "catch2/catch_test_macros.hpp"
#include "code/Grammar.hpp"
#include "tao/pegtl.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"
#include "tao/pegtl/memory_input.hpp"
#include "tao/pegtl/rules.hpp"
#include "tao/pegtl/string_input.hpp"

#include <initializer_list>
#include <iostream>
#include <memory>
#include <type_traits>

namespace p = tao::pegtl;

template<typename Rule>
const auto genericParse = [](const std::string& str)
{
	auto input = p::memory_input{str, "tests"};
	return p::parse_tree::parse<p::seq<Rule, p::eof>, ham::code::Selector>(input
	);
};

const auto decompose =
	[](std::unique_ptr<p::parse_tree::node>&& node, std::vector<int> indices)
{
	for (int i = 0; i < indices.size(); i++) {
		if (node->children.empty())
			return std::unique_ptr<p::parse_tree::node>{};
		node = std::move(node->children[indices[i]]);
	}
	return node;
};

TEST_CASE("Identifiers", "[grammar]")
{
	const auto parse = genericParse<ham::code::Identifier>;

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

TEST_CASE("Variables", "[grammar]")
{
	const auto parse = genericParse<ham::code::Variable>;

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
		const auto var = decompose(parse(str), {0});
		REQUIRE(var->children.size() == 1);
		REQUIRE(var->children[0]->string() == "Variable");
	}
}
