#ifndef HAM_DATA_DOMAIN_HPP
#define HAM_DATA_DOMAIN_HPP

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
class Scope {
  public:
	Scope() = delete;
	Scope(std::vector<VariableMap>& scopes, std::size_t level)
		: scopes(scopes),
		  level{level} {};

	std::optional<const List*> Find(std::string_view var);

	template<class A, class B>
	void Set(std::string_view&& var, std::unique_ptr<List>&& list)
	{
		scopes[level].insert_or_assign(
			std::forward<A>(var),
			std::forward<B>(list)
		);
	}

  private:
	std::vector<VariableMap>& scopes;
	std::size_t level;
};

/**
 * Holds data for individual domains. Must outlive all generated domains.
 */
class Domain {
  public:
	Scope GlobalScope() { return Scope(scopes, 0); }

  private:
	std::vector<VariableMap> scopes;
};

} // namespace ham::data

#endif // HAM_DATA_DOMAIN_HPP
