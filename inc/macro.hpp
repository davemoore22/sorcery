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

#pragma once

#include "main.hpp"

namespace Sorcery {

	// Macro to convert bool to C string
	inline auto BOOL2OPTIONCSTR(bool bool_to_convert) -> const char * {

		return bool_to_convert ? "on" : "off";
	}

	// Macro to convert std::string to C string
	inline auto CSTR(const std::string &string) -> const char * {

		return !string.empty() ? (char *)string.c_str() : (char *)"";
	}

	// Macro to create a GUID (Linux Only!)
	inline auto GUID() -> std::string {

		uuid_t uuidObj;
		uuid_generate(uuidObj);
		std::string value{std::begin(uuidObj), std::end(uuidObj)};

		return value;
	}

	// Pad a string to the desired length
	inline auto PADSTR(std::string &string, unsigned int width, bool pad_both = false)
		-> std::string {
		if (static_cast<unsigned int>(string.size()) < width) {
			const std::string::size_type padding{
				pad_both ? (width - string.size()) / 2 : (width - string.size())};
			std::string string_copy{string};
			if (pad_both)
				string_copy.insert(0, padding, ' ');
			string_copy.append(padding, ' ');
			return string_copy;
		} else
			return string;
	}

	// String trim Functions
	// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

	// Trim a string
	inline auto LTRIM(std::string &s) -> void {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
			return !std::isspace(ch);
		}));
	}

	// trim from end (in place)
	inline auto RTRIM(std::string &s) -> void {
		s.erase(std::find_if(s.rbegin(), s.rend(),
					[](unsigned char ch) {
						return !std::isspace(ch);
					})
					.base(),
			s.end());
	}

	// trim from both ends (in place)
	inline auto TRIM(std::string &s) -> void {
		LTRIM(s);
		RTRIM(s);
	}

	// trim from start (copying)
	inline auto LTRIM_COPY(std::string s)
		-> std::string { // NOLINT(clang-diagnostic-unused-function)
		LTRIM(s);
		return s;
	}

	// trim from end (copying)
	inline auto RTRIM_COPY(std::string s)
		-> std::string { // NOLINT(clang-diagnostic-unused-function)
		RTRIM(s);
		return s;
	}

	// trim from both ends (copying)
	inline auto TRIM_COPY(std::string s)
		-> std::string { // NOLINT(clang-diagnostic-unused-function)
		TRIM(s);
		return s;
	}

	// Wrap text (https://www.cplusplus.com/forum/beginner/132223/)
	inline auto WORDWRAP(std::string text, unsigned per_line) -> std::string {

		unsigned line_begin{0};
		while (line_begin < text.size()) {
			const unsigned int ideal_end{line_begin + per_line};
			unsigned int line_end = ideal_end <= text.size() ? ideal_end : text.size() - 1;

			if (line_end == text.size() - 1)
				++line_end;
			else if (std::isspace(text[line_end])) {
				text[line_end] = '@';
				++line_end;
			} else {
				// backtrack
				unsigned end = line_end;
				while ((end > line_begin) && (!std::isspace(text[end])))
					--end;

				if (end != line_begin) {
					line_end = end;
					text[line_end++] = '@';
				} else
					text.insert(line_end++, 1, '@');
			}

			line_begin = line_end;
		}

		return text;
	}

} // namespace Sorcery
