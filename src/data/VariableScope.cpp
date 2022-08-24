#include "data/VariableScope.hpp"

#include "data/Types.hpp"

#include <functional>
#include <memory>
#include <string_view>

namespace ham::data
{

std::optional<std::reference_wrapper<List>>
VariableScope::Find(std::string_view var)
{
	auto entry = var_map.find(var);
	if (entry != var_map.end()) {
		return entry->second;
	}

	if (parent) {
		return parent.value().get().Find(var);
	}

	return {};
}

VariableScope
VariableScope::CreateSubscope()
{
	VariableScope scope{};
	scope.parent = *this;
	return scope;
}

} // namespace ham::data
