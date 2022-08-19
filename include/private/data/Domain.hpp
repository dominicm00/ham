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

class Domain {
  public:
	Domain() = delete;
	Domain(std::vector<Scope>& scopes)
		: scopes(scopes){};

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
	std::vector<Scope>& scopes;
	std::size_t level;
};

} // namespace ham::data

#endif // HAM_DATA_DOMAIN_HPP
