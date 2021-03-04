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
} // namespace Sorcery
