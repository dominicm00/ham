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

	template<class M>
	void Set(std::string&& var, M&& list)
	{
		var_map.insert_or_assign(std::move(var), std::forward<M>(list));
	}

	std::optional<std::reference_wrapper<const List>> Find(std::string_view var
	);
	VariableScope CreateSubscope();

  private:
	std::optional<std::reference_wrapper<VariableScope>> parent;
	VariableMap var_map;
};

} // namespace ham::data

#endif // HAM_DATA_VARIABLESCOPE_HPP
