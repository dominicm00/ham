#ifndef HAM_UTIL_STRING_HPP
#define HAM_UTIL_STRING_HPP

#include <algorithm>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

namespace ham::util
{

class String
{
  public:
	std::string ToUpper(std::string_view str)
	{
		return std::ranges::transform(str, std::toupper);
	}

	std::string ToLower(std::string_view str)
	{
		return std::ranges::transform(str, std::tolower);
	}
};

} // namespace ham::util

#endif // HAM_UTIL_STRING_HPP
