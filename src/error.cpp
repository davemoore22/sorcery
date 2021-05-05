// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "error.hpp"

Sorcery::Error::Error(
	Enums::System::Error error_code, std::exception exception, std::string notes = {})
	: _error_code{error_code}, _exception{exception}, _notes{notes} {

	_timestamp = std::chrono::system_clock::now();

	_details.clear();
	_details.emplace_back(std::to_string(magic_enum::enum_integer(_error_code)));
	_details.emplace_back(magic_enum::enum_name(_error_code));
	_details.emplace_back(_exception.what());
	_details.emplace_back(get_when());
	_details.emplace_back(_notes);
}

auto Sorcery::Error::get_code() -> Enums::System::Error {

	return _error_code;
}

auto Sorcery::Error::get_exception() -> std::exception & {

	return _exception;
}

auto Sorcery::Error::get_what() -> std::string {

	return std::string{_exception.what()};
}

auto Sorcery::Error::get_when() -> std::string {

	auto in_time_t{std::chrono::system_clock::to_time_t(_timestamp)};
	std::stringstream ss{};
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
	return ss.str();
}

auto Sorcery::Error::get_notes() -> std::string {

	return _notes;
}

auto Sorcery::Error::get() -> std::vector<std::string> {

	return _details;
}

auto Sorcery::operator<<(std::ostream &out_stream, const Sorcery::Error &error) -> std::ostream & {

	out_stream << fmt::format(
					  "{:<16}:{}/{}", "ERROR CODE/TYPE", error._details[0], error._details[1])
			   << std::endl;
	out_stream << fmt::format("{:<16}:{}", "EXCEPTION", error._details[2]) << std::endl;
	out_stream << fmt::format("{:<16}:{}", "TIMESTAMP", error._details[3]) << std::endl;
	out_stream << fmt::format("{:<16}:{}", "DETAILS:", error._details[4]) << std::endl;

	return out_stream;
}