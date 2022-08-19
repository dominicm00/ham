#include "data/Domain.hpp"

#include "data/Types.hpp"

#include <memory>
#include <string_view>

namespace ham::data
{

std::optional<const List*>
Domain::Find(std::string_view var)
{
	for (auto i = level; i >= 0; i--) {
		Scope& scope = scopes[i];
		auto entry = scope.find(var);
		if (entry != scope.end()) {
			return entry->second.get();
		}
	}

	return {};
}

} // namespace ham::data
