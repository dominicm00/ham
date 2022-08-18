#include "parse/NodeParser.hpp"

#include "HamError.hpp"

#include <string>

namespace ham::parse
{

HamError
NodeParser::ConvertToHamError(tao::pegtl::parse_error pegtl_err)
{
	Position pos{};

	for (auto pegtl_file_pos : pegtl_err.positions()) {
		FilePosition file_pos;
		file_pos.file = pegtl_file_pos.source;
		file_pos.byte = pegtl_file_pos.byte;
		file_pos.line = pegtl_file_pos.line;
		file_pos.column = pegtl_file_pos.column;

		pos.FilePositions().push_back(file_pos);
	}

	return HamError(HamError::PARSE, pos, std::string{pegtl_err.message()});
}

} // namespace ham::parse
