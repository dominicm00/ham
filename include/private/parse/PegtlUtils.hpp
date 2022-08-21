#ifndef HAM_PARSE_PEGTLUTILS_HPP
#define HAM_PARSE_PEGTLUTILS_HPP

#include "tao/pegtl/position.hpp"
#include "util/HamError.hpp"

namespace ham::parse
{

namespace p = tao::pegtl;

FilePosition ConvertToHamFilePosition(p::position);
Position ConvertToHamPosition(std::vector<p::position>);

}

#endif // HAM_PARSE_PEGTLUTILS_HPP
