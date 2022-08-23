#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "parse/Grammar.hpp"
#include "tao/pegtl/contrib/trace.hpp"
#include "tao/pegtl/string_input.hpp"
#include "tests/ParseUtils.hpp"

namespace ham::tests
{

using namespace ham::parse;
const auto parse = genericParse<BracketExpression>;

TEST_CASE("Bracket expressions are non-empty", "[grammar]")
{
	auto stmt = GENERATE("", "[]", "[ ]");
	INFO(stmt);
	REQUIRE_THROWS(parse(stmt));
}

TEST_CASE("Bracket expressions don't contain non-value structures", "[grammar]")
{
	auto stmt = GENERATE(
		"[ while x { } ]",
		"[ for x in $(a) { } ]",
		"[ if x { } else { } ]"
	);
	INFO(stmt);
	REQUIRE_THROWS(parse(stmt));
}

TEST_CASE("Bracket expression, rule invocation", "[grammar]")
{
	REQUIRE_PARSE("[ Rule x ]", T<RuleActionInvocation>("Rule x"));
}

TEST_CASE("Bracket expression, target invocation", "[grammar]")
{
	REQUIRE_PARSE(
		"[ on target Rule x ]",
		T<TargetRuleInvocation>(
			{T<Leaf>("target"), T<RuleActionInvocation>("Rule x")}
		)
	);
}

} // namespace ham::tests
