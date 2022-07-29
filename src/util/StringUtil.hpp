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
	static std::string ToUpper(std::string_view str)
	{
		std::string ret{str};
		std::transform(
			ret.cbegin(),
			ret.cend(),
			ret.begin(), // write to the same location
			[](unsigned char c) { return std::toupper(c); }
		);
		return ret;
	}

	static std::string ToLower(std::string_view str)
	{
		std::string ret{str};
		std::transform(
			ret.cbegin(),
			ret.cend(),
			ret.begin(), // write to the same location
			[](unsigned char c) { return std::tolower(c); }
		);
		return ret;
	}
};

} // namespace ham::util

#endif // HAM_UTIL_STRING_HPP
