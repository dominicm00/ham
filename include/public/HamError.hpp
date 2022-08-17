#ifndef HAM_HAM_ERROR
#define HAM_HAM_ERROR

#include <cstddef>
#include <exception>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
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
	Position(FilePosition file_position)
		: file_positions{file_position} {};

	void AddFilePosition(FilePosition file_position)
	{
		file_positions.push_back(file_position);
	}

	std::vector<FilePosition> FilePositions() const noexcept
	{
		return file_positions;
	}

	friend std::ostream& operator<<(std::ostream& os, const Position& pos);

  private:
	std::vector<FilePosition> file_positions;
};

class HamError : std::exception {
  public:
	enum Phase {
		PARSE,
		EVALUATE,
		MAKE
	};

  public:
	HamError() = delete;
	HamError(
		Phase phase,
		std::optional<Position> position,
		std::string message
	);
	HamError(Phase phase, std::string message);

	const char* what() const noexcept override;

  private:
	std::string error_str;
};

} // namespace ham

#endif // HAM_HAM_ERROR
