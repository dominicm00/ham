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
#include <utility>
#include <vector>

namespace ham
{

struct FilePosition {
	std::size_t byte{};
	std::size_t column{};
	std::size_t line{};
	std::string file{};
};

class Position {
  public:
	Position() = delete;
	Position(FilePosition start, std::optional<FilePosition> end = {})
		: start(std::move(start)),
		  end(std::move(end)){};

	friend std::ostream& operator<<(std::ostream&, const Position&);

	const FilePosition start;
	const std::optional<FilePosition> end;
};

class HamError : public virtual std::exception {
  public:
	HamError() = delete;
	HamError(std::optional<Position> position, std::string_view message);
	HamError(std::string_view message);

	HamError(const HamError& other) noexcept = default;
	HamError& operator=(const HamError& other) noexcept = default;
	HamError(HamError&& other) noexcept = default;
	HamError& operator=(HamError&& other) noexcept = default;

	~HamError() override = default;

	[[nodiscard]] const char* what() const noexcept override;

  private:
	// Use shared_ptr here to prevent exceptions with copying
	std::shared_ptr<const std::string> error_str;
};

void
HamWarning(code::GlobalContext&, const Position&, std::string_view);

} // namespace ham

#endif // HAM_HAM_ERROR
