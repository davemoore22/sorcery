// Copyright (C) 2020 Dave Moore
//
// This file is part of Sorcery.
//
// Sorcery is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#pragma once

#include "main.hpp"

namespace Sorcery  {

	// Macro to convert bool to C string
	inline auto BOOL2OPTIONCSTR(bool bool_to_convert) -> const char* {
		return bool_to_convert ? "on" : "off";
	}

	// Macro to convert std::string to C string
	inline auto CSTR(const std::string &string_to_convert) -> const char* {
		return !string_to_convert.empty() ? (char *)string_to_convert.c_str() : (char *)"";
	}

	// Macro to convert std::string to Wide C string
	inline auto WCSTR(const std::string &string_to_convert) -> const wchar_t* {
		return !string_to_convert.empty() ? (wchar_t *)string_to_convert.c_str() : (wchar_t *)"";
	}

	// Macro to convert std::string_view to C string
	inline auto CSTRV(std::string_view string_to_convert) -> const char* {
		return !string_to_convert.empty() ? (char *)std::string(string_to_convert).c_str() : (char *)"";
	}
}