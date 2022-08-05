#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<integer>;

TEST_CASE("Integers are accepted", "[grammar]")
{
	auto num = GENERATE("1", "01", "00001", "99924327");
	REQUIRE(parse(num));
}

TEST_CASE("Floats are not accepted", "[grammar]")
{
	auto num = GENERATE("0.01", ".01", ".1");
	REQUIRE_FALSE(parse(num));
}

} // namespace ham::tests
