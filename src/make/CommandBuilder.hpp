/*
 * Copyright 2022, Dominic Martinez, dom@dominicm.dev.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_MAKE_PROCESSOR_HPP
#define HAM_MAKE_PROCESSOR_HPP

#include "data/RuleActions.hpp"
#include "make/Processor.hpp"

namespace ham::make
{

class Piecemeal
{
  public:
	/**
	 * Create a runnable Command from an actions call.
	 *
	 * \param[in] actionsCall
	 * \param[out] commands CommandList to add actions to
	 */
	static void
	BuildCommands(data::RuleActionsCall* actionsCall, CommandList& commands);

  private:
	/**
	 * Create a list of bound paths from an actions targets. Depending on action
	 * modifiers, may exclude certain targets.
	 *
	 * \param[in] actionsCall actions call to bind
	 * \param[in] isSources whether or not to bind the sources of the action
	 * call \param[out] boundTargets list to append bound targets to
	 */
	static void _BindActionsTargets(
		data::RuleActionsCall* actionsCall,
		bool isSources,
		StringList& boundTargets
	);

	/**
	 * Piecemeal a list of words based on a source list and max line length.
	 */
	static data::StringListList _PiecemealWords(
		code::EvaluationContext* context,
		std::vector<std::pair<std::string_view, std::string_view>> words,
		StringList boundSources,
		std::size_t maxLine
	);
};

} // namespace ham::make

#endif // HAM_MAKE_PROCESSOR_HPP
