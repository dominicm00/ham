#ifndef HAM_HAM_ERROR
#define HAM_HAM_ERROR

#include "code/Context.hpp"

#include <cstddef>
#include <exception>
#include <initializer_list>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace ham
{

struct FilePosition {
	std::size_t byte;
	std::size_t column;
	std::size_t line;
	std::string file;
};

class Position {
  public:
	Position() = delete;
	Position(FilePosition start, std::optional<FilePosition> end = {})
		: start(start),
		  end(end){};

	friend std::ostream& operator<<(std::ostream& os, const Position& pos);

  public:
	const FilePosition start;
	const std::optional<FilePosition> end;
};

class HamError : public std::exception {
  public:
	HamError() = delete;
	HamError(std::optional<Position> position, std::string_view message);
	HamError(std::string_view message);

	HamError(const HamError& other) noexcept = default;

	const char* what() const noexcept override;

  private:
	std::string error_str;
};

void
HamWarning(code::GlobalContext&, Position, std::string_view);

} // namespace ham

#endif // HAM_HAM_ERROR
