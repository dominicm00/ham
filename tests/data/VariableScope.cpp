#include "data/VariableScope.hpp"

#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "data/Types.hpp"

#include <initializer_list>
#include <memory>

namespace ham::tests
{

TEST_CASE("Variable scopes", "[data]")
{
	data::VariableScope scope{};
	data::List list = {"1", "2", "3"};
	scope.Set("X", list);

	SECTION("Non-existing variables")
	{
		std::string var = GENERATE("JAM_VERSION", "HAM_VERSION", "A", "A-zB_");
		REQUIRE_FALSE(scope.Find(var));
	}

	SECTION("Retrieve existing variable")
	{
		REQUIRE(scope.Find("X").value().get() == list);
	}

	SECTION("Subscopes")
	{
		data::VariableScope subscope = scope.CreateSubscope();
		data::List sublist = {"a", "b", "c"};
		subscope.Set("1", sublist);

		SECTION("Non-existing variables")
		{
			std::string var =
				GENERATE("JAM_VERSION", "HAM_VERSION", "A", "A-zB_");
			REQUIRE_FALSE(subscope.Find(var));
		}

		SECTION("Lookup in current scope")
		{
			REQUIRE(subscope.Find("1").value().get() == sublist);
		}

		SECTION("Lookup in parent scope")
		{
			REQUIRE(subscope.Find("X").value().get() == list);
		}

		SECTION("Can't look in child scope") { REQUIRE_FALSE(scope.Find("1")); }
	}
}

} // namespace ham::tests
