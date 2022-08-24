#include "code/Variable.hpp"

#include "catch2/catch_message.hpp"
#include "catch2/catch_test_macros.hpp"
#include "code/HamNodes.hpp"
#include "code/Identifier.hpp"
#include "data/Types.hpp"
#include "parse/Grammar.hpp"
#include "tests/CodeUtils.hpp"
#include "tests/ParseUtils.hpp"

namespace ham::tests
{

TEST_CASE("Variables", "[code]")
{
	auto eval_context = DefaultEvaluationContext();
	eval_context.variable_scope.Set("X", data::List{"a", "b", "c"});

	SECTION("ID only")
	{
		REQUIRE(
			Eval<parse::Variable, code::Variable>(
				eval_context,
				{N<parse::Identifier, code::Identifier>({"X"})}
			)
			== data::List{"a", "b", "c"}
		);
	}

	SECTION("Subscripts")
	{
		auto evalSubscript = [&eval_context](std::string subscript
							 ) -> data::List
		{
			return Eval<parse::Variable, code::Variable>(
				eval_context,
				{N<parse::Identifier, code::Identifier>({"X"}),
				 W<parse::Subscript>(
					 {N<parse::Identifier, code::Identifier>({subscript})}
				 )}
			);
		};

		auto hasWarning = [evalSubscript](std::string subscript) -> bool
		{
			REQUIRE(evalSubscript(subscript) == data::List{});
			UNSCOPED_INFO(last_warning);
			return last_warning.starts_with("warning: 1:1: the subscript");
		};

		SECTION("Valid subscripts")
		{
			REQUIRE(evalSubscript("1") == data::List{"a"});
			REQUIRE(evalSubscript("2") == data::List{"b"});
			REQUIRE(evalSubscript("3") == data::List{"c"});
			REQUIRE(evalSubscript("1-") == data::List{"a", "b", "c"});
			REQUIRE(evalSubscript("2-") == data::List{"b", "c"});
			REQUIRE(evalSubscript("3-") == data::List{"c"});
			REQUIRE(evalSubscript("1-2") == data::List{"a", "b"});
			REQUIRE(evalSubscript("2-3") == data::List{"b", "c"});
			REQUIRE(evalSubscript("3-3") == data::List{"c"});
			REQUIRE(evalSubscript("1-1") == data::List{"a"});
		}

		SECTION("Invalid subscripts")
		{
			REQUIRE_THROWS(evalSubscript("-"));
			REQUIRE_THROWS(evalSubscript("-1"));
			REQUIRE_THROWS(evalSubscript("--1"));
			REQUIRE_THROWS(evalSubscript("1--1"));
			REQUIRE_THROWS(evalSubscript("1-_1"));
			REQUIRE_THROWS(evalSubscript("1-1_"));
			REQUIRE_THROWS(evalSubscript("1_"));
			REQUIRE_THROWS(evalSubscript("_1"));
			REQUIRE_THROWS(evalSubscript("-_1"));
			REQUIRE_THROWS(evalSubscript("0"));
			REQUIRE_THROWS(evalSubscript("0-"));
			REQUIRE_THROWS(evalSubscript("0-0"));
			REQUIRE_THROWS(evalSubscript("1-0"));
		}

		SECTION("Out of bounds subscripts")
		{
			REQUIRE(hasWarning("4"));
			REQUIRE(hasWarning("1-4"));
			REQUIRE(hasWarning("3-4"));
			REQUIRE(hasWarning("4-4"));
			REQUIRE(hasWarning("4-2"));
			REQUIRE(hasWarning("3-2"));
			REQUIRE(hasWarning("4-"));
		}
	}

	// TODO: modifier tests
}

} // namespace ham::tests
