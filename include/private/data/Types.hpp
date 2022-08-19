#ifndef HAM_DATA_TYPES_HPP
#define HAM_DATA_TYPES_HPP

#include "tao/pegtl.hpp"

#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace ham
{

namespace p = tao::pegtl;

namespace data
{

// Common types used internally in Ham
using List = std::vector<std::string>;
using VariableMap = std::map<std::string, std::unique_ptr<List>, std::less<>>;

}

} // namespace ham

#endif // HAM_DATA_TYPES_HPP
