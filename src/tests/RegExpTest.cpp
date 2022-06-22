/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "tests/RegExpTest.hpp"

#include <utility>
#include <vector>

#include "data/RegExp.hpp"

namespace ham
{
namespace tests
{

using data::RegExp;

static std::vector<std::pair<size_t, size_t>>
match_result_to_vector(const RegExp::MatchResult& result)
{
	std::vector<std::pair<size_t, size_t>> vector;
	if (!result.HasMatched())
		return vector;

	vector.push_back(std::make_pair(result.StartOffset(), result.EndOffset()));

	size_t groupCount = result.GroupCount();
	for (size_t i = 0; i < groupCount; i++) {
		vector.push_back(std::make_pair(result.GroupStartOffsetAt(i),
										result.GroupEndOffsetAt(i)));
	}
	return vector;
}

struct RegExpTest::Matches : public std::vector<std::pair<size_t, size_t>> {
	Matches& Add(size_t offset, size_t length)
	{
		push_back(std::make_pair(offset, length));
		return *this;
	}

	const std::vector<std::pair<size_t, size_t>>& ToVector() const
	{
		return *this;
	}

	std::string ToString() const
	{
		return ValueToString<std::vector<std::pair<size_t, size_t>>>(*this);
	}
};

void
RegExpTest::Constructor()
{
	// default constructor
	{
		RegExp regExp;
		HAM_TEST_VERIFY(!regExp.IsValid())
		HAM_TEST_VERIFY(!regExp.Match("dummy").HasMatched())
	}

	// (const char*) constructor
	{
		RegExp regExp("dummy");
		HAM_TEST_VERIFY(regExp.IsValid())
		HAM_TEST_VERIFY(regExp.Match("dummy").HasMatched())
	}

	// copy constructor
	{
		RegExp regExp;
		RegExp regExp2(regExp);
		HAM_TEST_VERIFY(!regExp2.IsValid())
		HAM_TEST_VERIFY(!regExp2.Match("dummy").HasMatched())
	}

	{
		RegExp regExp("dummy");
		RegExp regExp2(regExp);
		HAM_TEST_VERIFY(regExp2.IsValid())
		HAM_TEST_VERIFY(regExp2.Match("dummy").HasMatched())
	}

	// More interesting test cases for the non-default constructors in the
	// Match() test.
}

void
RegExpTest::MatchRegularExpression()
{
	struct TestData {
		const char* pattern;
		const char* string;
		Matches matches;
	};

	const TestData testData[] = {
		{"", "", Matches().Add(0, 0)},
		{"", "foo", Matches().Add(0, 0)},
		{"foo", "bar", Matches()},
		{"foo", "foobar", Matches().Add(0, 3)},
		{"foo", "barfoo", Matches().Add(3, 6)},
		{"foo", "foobarfoo", Matches().Add(0, 3)},
		{".*foo", "foobarfoo", Matches().Add(0, 9)},
		{"foo(.*)foo", "foobarfoo", Matches().Add(0, 9).Add(3, 6)},
		{"foo(.*)?foo", "foobarfoo", Matches().Add(0, 9).Add(3, 6)},
		{"foo(.*)foo", "foofoo", Matches().Add(0, 6).Add(3, 3)},
		{"foo(bar)?foo", "foofoo", Matches().Add(0, 6)},
		{"foo(bar)?foo", "foobapfoo", Matches()},
		{"foo[bar]*foo", "foobapfoo", Matches()},
		{"foo[bar]*foo", "foobarfoo", Matches().Add(0, 9)},
		{"foo[^fub]*foo", "foobarfoo", Matches()},
		{"foo[^fuz]*foo", "foobarfoo", Matches().Add(0, 9)},
		{"foo[^fuz]+foo", "foobarfoo", Matches().Add(0, 9)},
		{"foo[^fuz]+foo", "foofoo", Matches()},
		{"(foo[^f]*)+", "foobarfoo", Matches().Add(0, 9).Add(6, 9)},
		{"(fo{3,4}[^f]*)+", "foobarfoo", Matches()},
		{"(fo{1,3}[^f]*)+", "foobarfoo", Matches().Add(0, 9).Add(6, 9)},
		{"f\\oo", "foobarfoo", Matches().Add(0, 3)},
		{"f\\[oo", "f[oobarfoo", Matches().Add(0, 4)},
		{"f\\*oo", "f*oobarfoo", Matches().Add(0, 4)},
		{"f\\?oo", "f?oobarfoo", Matches().Add(0, 4)},
		{"f\\.oo", "f.oobarfoo", Matches().Add(0, 4)},
		{"f\\{oo", "f{oobarfoo", Matches().Add(0, 4)},
		{"f\\^oo", "f^oobarfoo", Matches().Add(0, 4)},
		{"f\\$oo", "f$oobarfoo", Matches().Add(0, 4)},
		{"f\\(oo", "f(oobarfoo", Matches().Add(0, 4)},
		{"f\\\\oo", "f\\oobarfoo", Matches().Add(0, 4)},
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		const char* pattern = testData[i].pattern;
		const char* string = testData[i].string;
		const Matches& matches = testData[i].matches;

		// constructor
		{
			RegExp regExp(pattern, RegExp::PATTERN_TYPE_REGULAR_EXPRESSION);
			HAM_TEST_ADD_INFO(HAM_TEST_VERIFY(regExp.IsValid()),
							  "pattern: \"%s\"",
							  pattern)

			RegExp::MatchResult result = regExp.Match(string);
			HAM_TEST_ADD_INFO(HAM_TEST_EQUAL(match_result_to_vector(result),
											 matches.ToVector()),
							  "pattern: \"%s\", string: \"%s\"",
							  pattern,
							  string)
		}

		// copy constructor
		{
			RegExp regExp0(pattern, RegExp::PATTERN_TYPE_REGULAR_EXPRESSION);
			RegExp regExp(regExp0);
			HAM_TEST_ADD_INFO(HAM_TEST_VERIFY(regExp.IsValid()),
							  "pattern: \"%s\"",
							  pattern)
			RegExp::MatchResult result = regExp.Match(string);
			HAM_TEST_ADD_INFO(HAM_TEST_EQUAL(match_result_to_vector(result),
											 matches.ToVector()),
							  "pattern: \"%s\", string: \"%s\"",
							  pattern,
							  string)
		}

		// SetPattern()
		{
			RegExp regExp;
			HAM_TEST_ADD_INFO(HAM_TEST_VERIFY(regExp.SetPattern(
								  pattern,
								  RegExp::PATTERN_TYPE_REGULAR_EXPRESSION)),
							  "pattern: \"%s\"",
							  pattern)
			HAM_TEST_ADD_INFO(HAM_TEST_VERIFY(regExp.IsValid()),
							  "pattern: \"%s\"",
							  pattern)
			RegExp::MatchResult result = regExp.Match(string);
			HAM_TEST_ADD_INFO(HAM_TEST_EQUAL(match_result_to_vector(result),
											 matches.ToVector()),
							  "pattern: \"%s\", string: \"%s\"",
							  pattern,
							  string)
		}

		{
			RegExp regExp("some pattern");
			HAM_TEST_ADD_INFO(HAM_TEST_VERIFY(regExp.SetPattern(
								  pattern,
								  RegExp::PATTERN_TYPE_REGULAR_EXPRESSION)),
							  "pattern: \"%s\"",
							  pattern)
			HAM_TEST_ADD_INFO(HAM_TEST_VERIFY(regExp.IsValid()),
							  "pattern: \"%s\"",
							  pattern)
			RegExp::MatchResult result = regExp.Match(string);
			HAM_TEST_ADD_INFO(HAM_TEST_EQUAL(match_result_to_vector(result),
											 matches.ToVector()),
							  "pattern: \"%s\", string: \"%s\"",
							  pattern,
							  string)
		}
	}
}

void
RegExpTest::MatchWildcard()
{
	struct TestData {
		const char* pattern;
		const char* string;
		Matches matches;
	};

	const TestData testData[] = {
		{"", "", Matches().Add(0, 0)},
		{"", "foo", Matches().Add(0, 0)},
		{"foo", "bar", Matches()},
		{"foo", "foobar", Matches().Add(0, 3)},
		{"foo", "barfoo", Matches().Add(3, 6)},
		{"foo", "foobarfoo", Matches().Add(0, 3)},
		{"*foo", "foobarfoo", Matches().Add(0, 9)},
		{"foo*foo", "foobarfoo", Matches().Add(0, 9)},
		{"foo[bar][bar][bar]foo", "foobapfoo", Matches()},
		{"foo[bar][bar][bar]foo", "foobarfoo", Matches().Add(0, 9)},
		{"foo[^fub][^fub][^fub]foo", "foobarfoo", Matches()},
		{"foo[^fuz][^fuz][^fuz]foo", "foobarfoo", Matches().Add(0, 9)},
		{"foo[^fuz]foo", "foofoo", Matches()},
		{"foo[^f]", "foobarfoo", Matches().Add(0, 4)},
		{"foo[a-c]", "foobarfoo", Matches().Add(0, 4)},
		{"foo[x-z]", "foobarfoo", Matches()},
		{"foo[^a-c]", "foobarfoo", Matches()},
		{"foo[^x-z]", "foobarfoo", Matches().Add(0, 4)},
		{"f\\oo", "foobarfoo", Matches().Add(0, 3)},
		{"f\\[oo", "f[oobarfoo", Matches().Add(0, 4)},
		{"f\\*oo", "f*oobarfoo", Matches().Add(0, 4)},
		{"f\\?oo", "f?oobarfoo", Matches().Add(0, 4)},
		{"f\\\\oo", "f\\oobarfoo", Matches().Add(0, 4)},
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		const char* pattern = testData[i].pattern;
		const char* string = testData[i].string;
		const Matches& matches = testData[i].matches;

		// constructor
		{
			RegExp regExp(pattern, RegExp::PATTERN_TYPE_WILDCARD);
			HAM_TEST_ADD_INFO(HAM_TEST_VERIFY(regExp.IsValid()),
							  "pattern: \"%s\"",
							  pattern)

			RegExp::MatchResult result = regExp.Match(string);
			HAM_TEST_ADD_INFO(HAM_TEST_EQUAL(match_result_to_vector(result),
											 matches.ToVector()),
							  "pattern: \"%s\", string: \"%s\"",
							  pattern,
							  string)
		}

		// copy constructor
		{
			RegExp regExp0(pattern, RegExp::PATTERN_TYPE_WILDCARD);
			RegExp regExp(regExp0);
			HAM_TEST_ADD_INFO(HAM_TEST_VERIFY(regExp.IsValid()),
							  "pattern: \"%s\"",
							  pattern)
			RegExp::MatchResult result = regExp.Match(string);
			HAM_TEST_ADD_INFO(HAM_TEST_EQUAL(match_result_to_vector(result),
											 matches.ToVector()),
							  "pattern: \"%s\", string: \"%s\"",
							  pattern,
							  string)
		}

		// SetPattern()
		{
			RegExp regExp;
			HAM_TEST_ADD_INFO(
				HAM_TEST_VERIFY(
					regExp.SetPattern(pattern, RegExp::PATTERN_TYPE_WILDCARD)),
				"pattern: \"%s\"",
				pattern)
			HAM_TEST_ADD_INFO(HAM_TEST_VERIFY(regExp.IsValid()),
							  "pattern: \"%s\"",
							  pattern)
			RegExp::MatchResult result = regExp.Match(string);
			HAM_TEST_ADD_INFO(HAM_TEST_EQUAL(match_result_to_vector(result),
											 matches.ToVector()),
							  "pattern: \"%s\", string: \"%s\"",
							  pattern,
							  string)
		}

		{
			RegExp regExp("some pattern");
			HAM_TEST_ADD_INFO(
				HAM_TEST_VERIFY(
					regExp.SetPattern(pattern, RegExp::PATTERN_TYPE_WILDCARD)),
				"pattern: \"%s\"",
				pattern)
			HAM_TEST_ADD_INFO(HAM_TEST_VERIFY(regExp.IsValid()),
							  "pattern: \"%s\"",
							  pattern)
			RegExp::MatchResult result = regExp.Match(string);
			HAM_TEST_ADD_INFO(HAM_TEST_EQUAL(match_result_to_vector(result),
											 matches.ToVector()),
							  "pattern: \"%s\", string: \"%s\"",
							  pattern,
							  string)
		}
	}
}

} // namespace tests
} // namespace ham
