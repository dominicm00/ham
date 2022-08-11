#include "catch2/catch_test_macros.hpp"
#include "parse/Grammar.hpp"
#include "tao/pegtl/contrib/analyze.hpp"

#include <cstddef>

namespace ham::tests
{

using namespace ham::parse;

TEST_CASE("Grammar passes analysis", "[grammar]")
{
	const std::size_t issues = p::analyze<HamGrammar>(1);
	REQUIRE(issues == 0);
}

} // namespace ham::tests
