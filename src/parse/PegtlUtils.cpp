#include "parse/PegtlUtils.hpp"

#include "util/HamError.hpp"

namespace ham::parse
{

FilePosition
ConvertToHamFilePosition(const p::position& pegtl_file_pos)
{
	FilePosition file_pos;
	file_pos.file = pegtl_file_pos.source;
	file_pos.byte = pegtl_file_pos.byte;
	file_pos.line = pegtl_file_pos.line;
	file_pos.column = pegtl_file_pos.column;
	return file_pos;
}

Position
ConvertToHamPosition(
	const p::position& start,
	const std::optional<p::position>& end
)
{
	if (end) {
		return {
			ConvertToHamFilePosition(start),
			ConvertToHamFilePosition(end.value())};
	}
	return {ConvertToHamFilePosition(start)};
}

} // namespace ham::parse
