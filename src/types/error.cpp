// Copyright (C) 2025 Dave Moore
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
#include "common/include.hpp"
#include "common/macro.hpp"

Sorcery::Error::Error(Enums::System::Error error_code,
					  std::exception &exception, std::string notes)
	: _error_code{error_code},
	  _exception{exception},
	  _notes{notes} {

	_timestamp = std::chrono::system_clock::now();

	_details.clear();
	_details.emplace_back(std::to_string(unenum(_error_code)));
	_details.emplace_back(magic_enum::enum_name(_error_code));
	_details.emplace_back(_exception.what());
	_details.emplace_back(get_when());
	_details.emplace_back(_notes);

	// If no notes are supplied, generate a stack trace instead
	if (_details[4].size() == 0) {

		backward::StackTrace st;
		st.load_here(32);

		backward::TraceResolver tr;
		tr.load_stacktrace(st);

		for (size_t i = 0; i < st.size(); ++i) {
			backward::ResolvedTrace trace{tr.resolve(st[i])};

			_details.emplace_back(
				fmt::format("#{} {} {} [{}]\n", i, trace.object_filename,
							trace.object_function, trace.addr));
		}

	} else {

		// Split the display lines
		auto wrapped_notes{WORDWRAP(_details[4], 80)};
		const std::regex regex(R"([@]+)");
		std::sregex_token_iterator it{wrapped_notes.begin(),
									  wrapped_notes.end(), regex, -1};
		std::vector<std::string> lines{it, {}};
		lines.erase(std::remove_if(lines.begin(), lines.end(),
								   [](std::string const &s) {
									   return s.size() == 0;
								   }),
					lines.end());
		for (auto line_of_text : lines) {
			_details.emplace_back(line_of_text);
		}
	}
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

namespace Sorcery {

auto operator<<(std::ostream &out_stream, const Sorcery::Error &error)
	-> std::ostream & {

	out_stream << fmt::format("{:>5}: {} - {}", "Error", error._details[0],
							  error._details[1])
			   << std::endl;
	out_stream << fmt::format("{:>5}: {}", "What", error._details[2])
			   << std::endl;
	out_stream << fmt::format("{:>5}: {}", "When", error._details[3])
			   << std::endl;
	out_stream << fmt::format("{:>5}: {}", "Info", error._details[4])
			   << std::endl;

	return out_stream;
}
} // namespace Sorcery
