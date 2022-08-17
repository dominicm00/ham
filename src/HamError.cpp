#include "HamError.hpp"

namespace ham
{

std::ostream&
operator<<(std::ostream& os, const Position& pos)
{
	for (auto file_pos : pos.FilePositions()) {
		os << file_pos.line << ':' << file_pos.column << ':';
	}
	return os;
}

HamError::HamError(Phase phase, std::string message)
	: HamError(phase, {}, message){};

HamError::HamError(
	Phase phase,
	std::optional<Position> position,
	std::string message
)
{
	std::stringstream error_ss{};

	std::string phase_str;
	switch (phase) {
		case PARSE:
			phase_str = "parsing";
			break;
		case EVALUATE:
			phase_str = "evaluation";
			break;
		case MAKE:
			phase_str = "ninja invocation";
			break;
	}

	if (position) {
		error_ss << position.value() << ' ';
	}

	error_ss << "error in " << phase_str << ": " << message;
	error_str = error_ss.str();
}

const char*
HamError::what() const noexcept
{
	return error_str.c_str();
}

} // namespace ham
