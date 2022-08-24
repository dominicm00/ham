#include "util/HamError.hpp"

#include "code/Context.hpp"

#include <sstream>

namespace ham
{

std::ostream&
operator<<(std::ostream& os, const Position& pos)
{
	auto file_pos = pos.start;
	os << file_pos.line << ':' << file_pos.column << ':';
	return os;
}

HamError::HamError(std::string_view message)
	: HamError({}, message){};

HamError::HamError(std::optional<Position> position, std::string_view message)
{
	std::stringstream error_ss{};

	error_ss << "error: ";
	if (position) {
		error_ss << position.value() << ' ';
	}

	error_ss << message;
	error_str = error_ss.str();
}

const char*
HamError::what() const noexcept
{
	return error_str.c_str();
}

void
HamWarning(
	code::GlobalContext& global_context,
	Position pos,
	std::string_view message
)
{
	global_context.warning_stream << "warning: " << pos << " " << message
								  << std::endl;
}

} // namespace ham
