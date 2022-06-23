/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_EVALUATION_EXCEPTION_HPP
#define HAM_CODE_EVALUATION_EXCEPTION_HPP

#include "util/TextFileException.hpp"

namespace ham::code
{

class EvaluationException : public util::TextFileException
{
  public:
	EvaluationException(
		const std::string& message = std::string(),
		const util::TextFilePosition& position = util::TextFilePosition()
	)
		: util::TextFileException(message, position)
	{
	}
};

} // namespace ham::code

#endif // HAM_CODE_EVALUATION_EXCEPTION_HPP
