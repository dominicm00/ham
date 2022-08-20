#ifndef HAM_DATA_TYPES_HPP
#define HAM_DATA_TYPES_HPP

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace ham::data
{

// Common types used internally in Ham
using List = std::vector<std::string>;
using VariableMap = std::map<std::string, List, std::less<>>;

} // namespace ham::data

#endif // HAM_DATA_TYPES_HPP
