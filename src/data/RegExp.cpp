/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "data/RegExp.hpp"

#include "data/StringBuffer.hpp"
#include "util/Referenceable.hpp"

#include <exception>
#include <memory>
#include <regex.h>
#include <stdexcept>
#include <vector>

namespace ham::data
{

RegExp::Exception::Exception(Type type) noexcept
	: std::runtime_error(""),
	  fType(type),
	  fRegError(0),
	  fCompiledExpression(nullptr),
	  fErrorMessage(nullptr)
{
}

RegExp::Exception::Exception(int error, const regex_t* expression)
	: std::runtime_error(""),
	  fType(REGEX_ERROR),
	  fRegError(error),
	  fCompiledExpression(expression),
	  fErrorMessage(nullptr)
{
	auto bufSize = regerror(fRegError, fCompiledExpression, nullptr, 0);
	fErrorMessage = new char[bufSize];
	regerror(fRegError, fCompiledExpression, fErrorMessage, bufSize);
	fCompiledExpression = nullptr;
}

RegExp::Exception::~Exception() noexcept
{
	if (fErrorMessage != nullptr)
		delete[] fErrorMessage;
}

const char*
RegExp::Exception::what() const noexcept
{
	switch (fType) {
		case EMPTY_BRACK:
			return "Empty bracket expression encountered.";
		case MISSING_BRACK:
			return "Missing end bracket.";
		case BAD_ESCAPE:
			return "Escape character occured at end of regex.";
		case UNKNOWN:
			return "Unknown regex exception occured";
		case REGEX_ERROR:
			return fErrorMessage;
		default:
			return nullptr;
	}
}

class RegExp::Data
{
  public:
	Data(const char* pattern, PatternType patternType)
	{
		// convert the shell pattern to a regular expression
		StringBuffer patternString;
		if (patternType == PATTERN_TYPE_WILDCARD) {
			while (*pattern != '\0') {
				char c = *pattern++;
				switch (c) {
					case '?':
						patternString += '.';
						continue;
					case '*':
						patternString += ".*";
						continue;
					case '[': {
						// find the matching ']' first
						const char* end = pattern;
						while (*end != ']') {
							if (*end++ == '\0') {
								throw Exception(Exception::MISSING_BRACK);
							}
						}

						if (pattern == end) {
							// Empty bracket expression. It will never match
							// anything. Strictly speaking this is not
							// considered an error, but we handle it like
							// one.
							throw Exception(Exception::EMPTY_BRACK);
						}

						patternString += '[';

						// We need to avoid "[." ... ".]", "[=" ... "=]", and
						// "[:" ... ":]" sequences, since those have special
						// meaning in regular expressions. If we encounter a '['
						// followed by either of '.', '=', or ':', we replace
						// the '[' by "[.[.]".
						while (pattern < end) {
							c = *pattern++;
							if (c == '[' && pattern < end) {
								switch (*pattern) {
									case '.':
									case '=':
									case ':':
										patternString += "[.[.]";
										continue;
								}
							}
							patternString += c;
						}

						pattern++;
						patternString += ']';
						break;
					}

					case '\\': {
						// Quotes the next character. Works the same way for
						// regular expressions.
						if (*pattern == '\0') {
							throw Exception(Exception::BAD_ESCAPE);
						}

						patternString += '\\';
						patternString += *pattern++;
						break;
					}

					case '^':
					case '.':
					case '$':
					case '(':
					case ')':
					case '|':
					case '+':
					case '{':
						// need to be quoted
						patternString += '\\';
						// fall through
					default:
						patternString += c;
						break;
				}
			}

			pattern = patternString.Data();
		}

		if (int errorCode =
				regcomp(&fCompiledExpression, pattern, REG_EXTENDED))
			throw Exception(errorCode, &fCompiledExpression);
	}

	~Data() { regfree(&fCompiledExpression); }

	const regex_t* CompiledExpression() const { return &fCompiledExpression; }

  private:
	regex_t fCompiledExpression;
};

RegExp::RegExp(const char* pattern, PatternType patternType)
	: fData(std::shared_ptr<Data>{new Data(pattern, patternType)})
{
}

RegExp::RegExp(const RegExp& other)
	: fData(other.fData)
{
}

RegExp::~RegExp() = default;

RegExp::MatchResult
RegExp::Match(const char* string) const
{
	return MatchResult(fData->CompiledExpression(), string);
}

RegExp&
RegExp::operator=(const RegExp& other)
{
	fData = other.fData;
	return *this;
}

RegExp::MatchResult::MatchResult(
	const regex_t* compiledExpression,
	const char* string
)
	: fMatchCount(0),
	  fMatches(nullptr)
{
	// Do the matching: Since we need to provide a buffer for the matches
	// for regexec() to fill in, but don't know the number of matches
	// beforehand, we need to guess and retry with a larger buffer, if it
	// wasn't large enough.
	size_t maxMatchCount = 32;
	for (;;) {
		fMatches = std::shared_ptr<regmatch_t[]>{new regmatch_t[maxMatchCount]};
		if (regexec(
				compiledExpression,
				string,
				maxMatchCount,
				fMatches.get(),
				0
			)
			!= 0) {
			// no matches were found
			fMatchCount = 0;
			fMatches.reset();
			break;
		}

		if (fMatches[maxMatchCount - 1].rm_so == -1) {
			// determine the match count
			size_t lower = 0;
			size_t upper = maxMatchCount;
			while (lower < upper) {
				size_t mid = (lower + upper) / 2;
				if (fMatches[mid].rm_so == -1)
					upper = mid;
				else
					lower = mid + 1;
			}
			fMatchCount = lower;
			break;
		}

		// buffer too small -- try again with larger buffer
		maxMatchCount *= 2;
	}

	if (fMatchCount > 0 && fMatches.get() == nullptr)
		throw Exception(Exception::UNKNOWN);
}

RegExp::MatchResult::MatchResult(const MatchResult& other)
	: fMatchCount(other.fMatchCount),
	  fMatches(other.fMatches)
{
}

RegExp::MatchResult::~MatchResult() noexcept = default;

bool
RegExp::MatchResult::HasMatched() const
{
	return fMatchCount > 0;
}

size_t
RegExp::MatchResult::StartOffset() const
{
	return fMatchCount > 0 ? fMatches[0].rm_so : 0;
}

size_t
RegExp::MatchResult::EndOffset() const
{
	return fMatchCount > 0 ? fMatches[0].rm_eo : 0;
}

size_t
RegExp::MatchResult::GroupCount() const
{
	return fMatchCount > 0 ? fMatchCount - 1 : 0;
}

size_t
RegExp::MatchResult::GroupStartOffsetAt(size_t index) const
{
	return fMatchCount > index + 1 ? fMatches[index + 1].rm_so : 0;
}

size_t
RegExp::MatchResult::GroupEndOffsetAt(size_t index) const
{
	return fMatchCount > index + 1 ? fMatches[index + 1].rm_eo : 0;
}

RegExp::MatchResult&
RegExp::MatchResult::operator=(const MatchResult& other)
{
	fMatchCount = other.fMatchCount;
	fMatches = other.fMatches;

	return *this;
}

} // namespace ham::data
