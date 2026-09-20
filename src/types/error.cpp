// Copyright (C) 2026 Dave Moore
//
// This file is part of Sorcery.
//
// Sorcery is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 2 of the License, or (at your option) any later
// version.
//
// Sorcery is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Sorcery.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "types/error.hpp"
#include "common/macro.hpp" // for WORDWRAP
#include "types/meta.hpp"	// for enum_name
#include <ctime>			// for localtime
#include <format>			// for format
#include <iomanip>			// for operator<<, put_time
#include <regex>			// for regex, regex_token_iterator, sregex_toke...
#include <sstream>			// for basic_ostream, basic_stringstream, opera...
#include <utility>			// for move, to_underlying

Sorcery::Error::Error(Enums::System::Error error_code, const std::exception &exception, std::string notes)
	: _error_code{error_code},
	  _what{exception.what()},
	  _timestamp{std::chrono::system_clock::now()},
	  _notes{std::move(notes)} {

	_details.emplace_back(std::to_string(std::to_underlying(_error_code)));

	_details.emplace_back(enum_name(_error_code));
	_details.emplace_back(_what);
	_details.emplace_back(get_when());
	_details.emplace_back(_notes);

	// Split the display lines
	auto wrapped_notes{WORDWRAP(_notes, 80)};

	const std::regex regex{R"([@]+)"};

	std::sregex_token_iterator it{wrapped_notes.begin(), wrapped_notes.end(), regex, -1};

	std::vector<std::string> lines{it, {}};

	std::erase_if(lines, [](const std::string &line) {
		return line.empty();
	});

	for (auto &line : lines)
		_details.emplace_back(std::move(line));
}
auto Sorcery::Error::get_code() const -> Enums::System::Error {

	return _error_code;
}

auto Sorcery::Error::get_what() const -> const std::string & {

	return _what;
}

auto Sorcery::Error::get_when() const -> std::string {

	const auto in_time_t{std::chrono::system_clock::to_time_t(_timestamp)};

	std::stringstream ss{};
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");

	return ss.str();
}

auto Sorcery::Error::get_notes() const -> const std::string & {

	return _notes;
}

auto Sorcery::Error::get() const -> const std::vector<std::string> & {

	return _details;
}

namespace Sorcery {

auto operator<<(std::ostream &out_stream, const Error &error) -> std::ostream & {

	out_stream << std::format("{:>5}: {} - {}", "Error", error._details[0], error._details[1]) << '\n';
	out_stream << std::format("{:>5}: {}", "What", error._details[2]) << '\n';
	out_stream << std::format("{:>5}: {}", "When", error._details[3]) << '\n';
	out_stream << std::format("{:>5}: {}", "Info", error._details[4]) << '\n';

	return out_stream;
}

} // namespace Sorcery