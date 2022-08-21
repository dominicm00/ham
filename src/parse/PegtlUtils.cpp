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
ConvertToHamPosition(std::vector<p::position> pegtl_pos)
{
	Position pos{};
	for (auto pegtl_file_pos : pegtl_pos) {
		pos.FilePositions().push_back(ConvertToHamFilePosition(pegtl_file_pos));
	}
	return pos;
}

} // namespace ham::parse
