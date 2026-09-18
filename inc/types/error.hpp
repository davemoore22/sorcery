#pragma once

#include "common/enum.hpp"

#include <chrono>
#include <exception>
#include <iosfwd>
#include <string>
#include <vector>

namespace Sorcery {

class Error {

	public:
		Error(Enums::System::Error error_code, const std::exception &exception, std::string notes = "");

		Error() = delete;

		friend auto operator<<(std::ostream &out_stream, const Error &error) -> std::ostream &;

		auto get_code() const -> Enums::System::Error;
		auto get_what() const -> const std::string &;
		auto get_when() const -> std::string;
		auto get_notes() const -> const std::string &;
		auto get() const -> const std::vector<std::string> &;

	private:
		Enums::System::Error _error_code;
		std::string _what;
		std::chrono::time_point<std::chrono::system_clock> _timestamp;
		std::vector<std::string> _details;
		std::string _notes;
};

} // namespace Sorcery