#include "parse/PegtlUtils.hpp"

#include "util/HamError.hpp"

namespace ham::parse
{

FilePosition
ConvertToHamFilePosition(p::position pegtl_file_pos)
{
	FilePosition file_pos;
	file_pos.file = pegtl_file_pos.source;
	file_pos.byte = pegtl_file_pos.byte;
	file_pos.line = pegtl_file_pos.line;
	file_pos.column = pegtl_file_pos.column;
	return file_pos;
}

Position
ConvertToHamPosition(p::position start, std::optional<p::position> end)
{
	if (end) {
		return Position(
			ConvertToHamFilePosition(start),
			ConvertToHamFilePosition(end.value())
		);
	} else {
		return Position(ConvertToHamFilePosition(start));
	}
}

} // namespace ham::parse
