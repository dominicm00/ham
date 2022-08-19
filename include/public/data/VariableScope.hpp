#ifndef HAM_DATA_VARIABLESCOPE_HPP
#define HAM_DATA_VARIABLESCOPE_HPP

#include "data/Types.hpp"

#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace ham::data
{

/**
 * References all accessible variables in a given scope.
 */
class VariableScope {
  public:
	VariableScope() = default;

	// Owns variable data; cannot be copied
	VariableScope(const VariableScope&) = delete;
	VariableScope& operator=(const VariableScope&) = delete;

	// May be moved
	VariableScope(VariableScope&&) = default;
	VariableScope& operator=(VariableScope&&) = default;

	template<class A, class B>
	void Set(std::string_view&& var, std::unique_ptr<List>&& list)
	{
		var_map.insert_or_assign(std::forward<A>(var), std::forward<B>(list));
	}

	std::optional<const List*> Find(std::string_view var);
	VariableScope CreateSubscope();

  private:
	std::optional<std::reference_wrapper<VariableScope>> parent;
	VariableMap var_map;
};

} // namespace ham::data

#endif // HAM_DATA_VARIABLESCOPE_HPP
