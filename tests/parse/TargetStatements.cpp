#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tao/pegtl/contrib/trace.hpp"
#include "tao/pegtl/string_input.hpp"
#include "tests/Utils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<target_statement>;

TEST_CASE("Target statements are non-empty", "[grammar]")
{
	auto stmt = GENERATE("", ";", " ;");
	REQUIRE_THROWS(parse(stmt));
}

TEST_CASE("Target statements, rule invocations", "[grammar]")
{
	REQUIRE_PARSE(
		"on target Echo $(X) ;",
		T<target_statement>(
			{T<leaf>("target"),
			 T<rule_invocation>({T<identifier>("Echo"), T<list>()})}
		)
	);
}

TEST_CASE("Target statements, control flow", "[grammar]")
{
	REQUIRE_PARSE(
		"on target for var in $(X) { Echo $(var) ; }",
		T<target_statement>({T<leaf>("target"), T<for_loop>()})
	);
}

} // namespace ham::tests
