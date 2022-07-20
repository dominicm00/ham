/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Copyright 2022, Dominic Martinez, dom@dominicm.dev.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_REG_EXP_HPP
#define HAM_DATA_REG_EXP_HPP

#include <memory>
#include <regex.h>
#include <stddef.h>
#include <stdexcept>

namespace ham::data
{

class RegExp
{
  public:
	enum PatternType {
		PATTERN_TYPE_REGULAR_EXPRESSION,
		PATTERN_TYPE_WILDCARD
	};

	class MatchResult;
	class Exception;

  public:
	RegExp(
		const char* pattern,
		PatternType patternType = PATTERN_TYPE_REGULAR_EXPRESSION
	);
	RegExp(const RegExp& other);
	~RegExp();

	MatchResult Match(const char* string) const;

	RegExp& operator=(const RegExp& other);

  private:
	class Data;

  private:
	std::shared_ptr<Data> fData;
};

class RegExp::Exception : public std::runtime_error
{
  public:
	enum Type {
		EMPTY_BRACK,   ///< empty bracket expression
		MISSING_BRACK, ///< missing end bracket
		BAD_ESCAPE,	   ///< escape character "\" occurred at end of regex
		REGEX_ERROR,   ///< error in regex method, error code in fRegError
		UNKNOWN		   ///< unknown exception occured
	};

	Exception(Type type) noexcept;
	Exception(int error, const regex_t* expression);
	~Exception() noexcept;

	const char* what() const noexcept override;

  private:
	const Type fType;
	const int fRegError;
	const regex_t* fCompiledExpression;
	char* fErrorMessage;
};

class RegExp::MatchResult
{
  public:
	MatchResult(const MatchResult& other);
	~MatchResult() noexcept;

	bool HasMatched() const;

	size_t StartOffset() const;
	size_t EndOffset() const;

	size_t GroupCount() const;
	size_t GroupStartOffsetAt(size_t index) const;
	size_t GroupEndOffsetAt(size_t index) const;

	MatchResult& operator=(const MatchResult& other);

  private:
	friend class RegExp;

  private:
	MatchResult(const regex_t* compiledExpression, const char* string);

  private:
	size_t fMatchCount;
	std::shared_ptr<regmatch_t[]> fMatches;
};

} // namespace ham::data

#endif // HAM_DATA_REG_EXP_HPP
