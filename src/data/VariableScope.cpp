#include "data/VariableScope.hpp"

#include "data/Types.hpp"

#include <memory>
#include <string_view>

namespace ham::data
{

std::optional<const List*>
VariableScope::Find(std::string_view var)
{
	auto entry = var_map.find(var);
	if (entry != var_map.end()) {
		return entry->second.get();
	} else if (parent) {
		return parent.value().get().Find(var);
	} else {
		return {};
	}
}

VariableScope
VariableScope::CreateSubscope()
{
	VariableScope scope{};
	scope.parent = *this;
	return std::move(scope);
}

} // namespace ham::data
