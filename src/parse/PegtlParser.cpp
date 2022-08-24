#include "parse/PegtlParser.hpp"

#include "data/Types.hpp"
#include "parse/PegtlUtils.hpp"
#include "util/HamError.hpp"

#include <string>

namespace ham::parse
{

HamError
PegtlParser::ConvertToHamError(const p::parse_error& pegtl_err)
{
	return {
		ConvertToHamPosition(pegtl_err.positions().back()),
		std::string{pegtl_err.message()}};
}

} // namespace ham::parse
