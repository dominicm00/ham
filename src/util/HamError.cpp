#include "util/HamError.hpp"

#include "code/Context.hpp"

#include <memory>
#include <sstream>

namespace ham
{

std::ostream&
operator<<(std::ostream& ostream, const Position& pos)
{
	auto file_pos = pos.start;
	ostream << file_pos.line << ':' << file_pos.column << ':';
	return ostream;
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
	error_str = std::make_shared<std::string>(error_ss.str());
}

const char*
HamError::what() const noexcept
{
	return error_str->c_str();
}

void
HamWarning(
	code::GlobalContext& global_context,
	const Position& pos,
	std::string_view message
)
{
	global_context.warning_stream << "warning: " << pos << " " << message
								  << std::endl;
}

} // namespace ham
