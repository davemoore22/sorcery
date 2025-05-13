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

#pragma once

#include "common/include.hpp"

// Enums
namespace Sorcery {

// Use the + Operator or unenum to get the int value of an enum class
template <typename T>
constexpr auto operator+(T e) noexcept
	-> std::enable_if_t<std::is_enum<T>::value, std::underlying_type_t<T>> {
	return static_cast<std::underlying_type_t<T>>(e);
}

template <class Enum>
constexpr std::underlying_type_t<Enum> unenum(Enum e) noexcept {
	return static_cast<std::underlying_type_t<Enum>>(e);
}

// Macro to create a GUID (Linux Only!)
inline auto GUID() -> std::string {

	// TODO: move to platform.h
	char guid[100];
	uuid_t uuidObj;
	uuid_generate(uuidObj);
	uuid_unparse(uuidObj, guid);
	std::string value{guid};

	return value;
}

inline auto COL2VEC(std::string_view colour) -> ImVec4 {

	const auto val{std::stoull(std::string{colour}, 0, 16)};
	const ImColor out{static_cast<ImU32>(val)};
	return ImVec4{out};
}

inline auto COL2NUM(std::string_view colour) -> ImU32 {

	const auto val{std::stoull(std::string{colour}, 0, 16)};
	return static_cast<ImU32>(val);
}

// String trim Functions from
// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

// Trim a string
inline auto LTRIM(std::string &s) -> void {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
				return !std::isspace(ch);
			}));
}

// Trim from end (in place)
inline auto RTRIM(std::string &s) -> void {
	s.erase(std::find_if(s.rbegin(), s.rend(),
						 [](unsigned char ch) {
							 return !std::isspace(ch);
						 })
				.base(),
			s.end());
}

// Trim from both ends (in place)
inline auto TRIM(std::string &s) -> void {
	LTRIM(s);
	RTRIM(s);
}

// Trim from end (copying)
inline auto RTRIM_COPY(std::string s)
	-> std::string { // NOLINT(clang-diagnostic-unused-function)
	RTRIM(s);
	return s;
}

// Trim from both ends (copying)
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
		unsigned int line_end =
			ideal_end <= text.size() ? ideal_end : text.size() - 1;

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

// Print
inline auto PRINT(const std::string &string) -> void {

	std::cout << string << std::endl;
}

#define PRINTFUNC std::cout << __PRETTY_FUNCTION__ << std::endl;

inline auto CAPITALISE(const std::string &str) {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
	std::string output;
	for (int i = 0; i < str.length(); i++) {
		char c = str[i];
		if (i == 0)
			output += std::toupper(c);
		else if (i != 0)
			output += (std::tolower(c));
	}

	return output;
#pragma GCC diagnostic pop
}

// Timepoint to String
inline auto TP2STR(const std::chrono::time_point<std::chrono::system_clock> tp)
	-> std::string {

	// Need to do it this way til std::chrono::locate_zone etc is supported
	auto t{std::chrono::system_clock::to_time_t(tp)};
	std::string ts{std::ctime(&t)};
	ts.resize(ts.size() - 1);
	return ts;
}

};