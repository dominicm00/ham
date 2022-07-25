/*
 * Copyright 2022, Dominic Martinez, dom@dominicm.dev.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_MAKE_PIECEMEAL_HPP
#define HAM_MAKE_PIECEMEAL_HPP

#include "code/EvaluationContext.hpp"
#include "data/RuleActions.hpp"
#include "make/Command.hpp"

#include <vector>

namespace ham::make
{

class Piecemeal
{
  public:
	/**
	 * Piecemeal a list of words based on a source list and max line length.
	 */
	static data::StringListList Words(
		code::EvaluationContext& externalContext,
		const std::string& actionName,
		const std::vector<std::pair<std::string_view, std::string_view>>& words,
		const StringList& boundSources,
		std::size_t maxLine
	);
};

} // namespace ham::make

#endif // HAM_MAKE_PIECEMEAL_HPP
