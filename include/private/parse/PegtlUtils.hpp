#ifndef HAM_PARSE_PEGTLUTILS_HPP
#define HAM_PARSE_PEGTLUTILS_HPP

#include "tao/pegtl/position.hpp"
#include "util/HamError.hpp"

namespace ham::parse
{

namespace p = tao::pegtl;

FilePosition
ConvertToHamFilePosition(const p::position&);

Position
ConvertToHamPosition(
	const p::position& start,
	const std::optional<p::position>& end = {}
);

}

#endif // HAM_PARSE_PEGTLUTILS_HPP
