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

namespace Sorcery {

// Macro to convert std::string to C string
inline auto CSTR(const std::string &string) -> const char * {

	return !string.empty() ? (char *)string.c_str() : (char *)"";
}

// Split a string -
// https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
inline auto SPLIT(const std::string &text) -> std::vector<std::string> {

	std::istringstream iss(text);
	std::vector<std::string> results((std::istream_iterator<std::string>(iss)),
									 std::istream_iterator<std::string>());

	return results;
}
}