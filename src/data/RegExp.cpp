/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "data/RegExp.hpp"

#include "data/StringBuffer.hpp"
#include "util/Referenceable.hpp"

#include <regex.h>
#include <vector>

namespace ham::data
{

// #pragma mark - RegExp::Data

class RegExp::Data
{
  public:
	Data(const char* pattern, PatternType patternType)
		: fReferenceCount(1)
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
								fError = REG_EBRACK;
								return;
							}
						}

						if (pattern == end) {
							// Empty bracket expression. It will never match
							// anything. Strictly speaking this is not
							// considered an error, but we handle it like one.
							fError = REG_EBRACK;
							return;
						}

						patternString += '[';

						// We need to avoid "[." ... ".]", "[=" ... "=]", and
						// "[:" ... ":]" sequences, since those have special
						// meaning in regular expressions. If we encounter
						// a '[' followed by either of '.', '=', or ':', we
						// replace the '[' by "[.[.]".
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
							fError = REG_EESCAPE;
							return;
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

		fError = regcomp(&fCompiledExpression, pattern, REG_EXTENDED);
	}

	~Data()
	{
		if (fError == 0)
			regfree(&fCompiledExpression);
	}

	void Acquire() { util::increment_reference_count(fReferenceCount); }

	void Release()
	{
		if (util::decrement_reference_count(fReferenceCount) == 1)
			delete this;
	}

	bool IsValid() const { return fError == 0; }

	const regex_t* CompiledExpression() const { return &fCompiledExpression; }

  private:
	int32_t fReferenceCount;
	int fError;
	regex_t fCompiledExpression;
};

// #pragma mark - RegExp::MatchResultData

class RegExp::MatchResultData
{
  public:
	MatchResultData(const regex_t* compiledExpression, const char* string)
		: fReferenceCount(1),
		  fMatchCount(0),
		  fMatches(nullptr)
	{
		// Do the matching: Since we need to provide a buffer for the matches
		// for regexec() to fill in, but don't know the number of matches
		// beforehand, we need to guess and retry with a larger buffer, if it
		// wasn't large enough.
		size_t maxMatchCount = 32;
		for (;;) {
			fMatches = new regmatch_t[maxMatchCount];
			if (regexec(compiledExpression, string, maxMatchCount, fMatches, 0)
				!= 0) {
				delete[] fMatches;
				fMatches = nullptr;
				fMatchCount = 0;
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
			delete[] fMatches;
			fMatches = nullptr;
			maxMatchCount *= 2;
		}
	}

	~MatchResultData() { delete[] fMatches; }

	void Acquire() { util::increment_reference_count(fReferenceCount); }

	void Release()
	{
		if (util::decrement_reference_count(fReferenceCount) == 1)
			delete this;
	}

	size_t MatchCount() const { return fMatchCount; }

	const regmatch_t* Matches() const { return fMatches; }

  private:
	int32_t fReferenceCount;
	size_t fMatchCount;
	regmatch_t* fMatches;
};

// #pragma mark - RegExp

RegExp::RegExp()
	: fData(nullptr)
{
}

RegExp::RegExp(const char* pattern, PatternType patternType)
	: fData(nullptr)
{
	SetPattern(pattern, patternType);
}

RegExp::RegExp(const RegExp& other)
	: fData(other.fData)
{
	if (fData != nullptr)
		fData->Acquire();
}

RegExp::~RegExp()
{
	if (fData != nullptr)
		fData->Release();
}

bool
RegExp::SetPattern(const char* pattern, PatternType patternType)
{
	if (fData != nullptr) {
		fData->Release();
		fData = nullptr;
	}

	fData = new Data(pattern, patternType);
	if (!fData->IsValid()) {
		delete fData;
		fData = nullptr;
		return false;
	}

	return true;
}

RegExp::MatchResult
RegExp::Match(const char* string) const
{
	if (!IsValid())
		return MatchResult();

	return MatchResult(new MatchResultData(fData->CompiledExpression(), string)
	);
}

RegExp&
RegExp::operator=(const RegExp& other)
{
	if (fData != nullptr)
		fData->Release();

	fData = other.fData;

	if (fData != nullptr)
		fData->Acquire();

	return *this;
}

// #pragma mark - RegExp::MatchResult

RegExp::MatchResult::MatchResult()
	: fData(nullptr)
{
}

RegExp::MatchResult::MatchResult(MatchResultData* data)
	: fData(data)
{
}

RegExp::MatchResult::MatchResult(const MatchResult& other)
	: fData(other.fData)
{
	if (fData != nullptr)
		fData->Acquire();
}

RegExp::MatchResult::~MatchResult()
{
	if (fData != nullptr)
		fData->Release();
}

bool
RegExp::MatchResult::HasMatched() const
{
	return fData != nullptr && fData->MatchCount() > 0;
}

size_t
RegExp::MatchResult::StartOffset() const
{
	return fData != nullptr && fData->MatchCount() > 0
		? fData->Matches()[0].rm_so
		: 0;
}

size_t
RegExp::MatchResult::EndOffset() const
{
	return fData != nullptr && fData->MatchCount() > 0
		? fData->Matches()[0].rm_eo
		: 0;
}

size_t
RegExp::MatchResult::GroupCount() const
{
	if (fData == nullptr)
		return 0;

	size_t matchCount = fData->MatchCount();
	return matchCount > 0 ? matchCount - 1 : 0;
}

size_t
RegExp::MatchResult::GroupStartOffsetAt(size_t index) const
{
	return fData != nullptr && fData->MatchCount() > index + 1
		? fData->Matches()[index + 1].rm_so
		: 0;
}

size_t
RegExp::MatchResult::GroupEndOffsetAt(size_t index) const
{
	return fData != nullptr && fData->MatchCount() > index + 1
		? fData->Matches()[index + 1].rm_eo
		: 0;
}

RegExp::MatchResult&
RegExp::MatchResult::operator=(const MatchResult& other)
{
	if (fData != nullptr)
		fData->Release();

	fData = other.fData;

	if (fData != nullptr)
		fData->Acquire();

	return *this;
}

} // namespace ham::data
