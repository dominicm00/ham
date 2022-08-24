#include "code/Identifier.hpp"

#include "catch2/catch_test_macros.hpp"
#include "code/HamNodes.hpp"
#include "code/Node.hpp"
#include "code/Variable.hpp"
#include "data/Types.hpp"
#include "parse/Grammar.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"
#include "tests/CodeUtils.hpp"

#include <memory>
#include <string>
#include <vector>

namespace ham::tests
{

TEST_CASE("Literal identifier", "[code]")
{
	data::List id = Eval<parse::Identifier, code::Identifier>(
		{E<parse::IdString>({"my_id"})}
	);
	REQUIRE(id == data::List{"my_id"});
}

TEST_CASE("Dynamic identifier", "[code]")
{
	data::List id = Eval<parse::Identifier, code::Identifier>(
		{N<parse::Variable, code::Variable>({"my"}),
		 E<parse::IdString>({"_id"})}
	);
	REQUIRE(id == data::List{"my_id"});
}

TEST_CASE("Id var with invalid characters", "[code]")
{
	REQUIRE_THROWS(Eval<parse::Identifier, code::Identifier>(
		{N<parse::Variable, code::Variable>({"invalid@"})}
	));
}

TEST_CASE("Id var with 0 elements", "[code]")
{
	REQUIRE_THROWS(Eval<parse::Identifier, code::Identifier>(
		{N<parse::Variable, code::Variable>({})}
	));
}

TEST_CASE("Id var with multiple elements", "[code]")
{
	REQUIRE_THROWS(Eval<parse::Identifier, code::Identifier>(
		{N<parse::Variable, code::Variable>({"a", "b"})}
	));
}

} // namespace ham::tests
