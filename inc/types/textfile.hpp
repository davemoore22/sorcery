// Copyright (C) 2024 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Shadows under Llylgamyn.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#pragma once

#include "main.hpp"

// Class to handles loading and displaying on the console the contents of text
// files (such as COPYING and README)
namespace Sorcery {

class TextFile {

	public:

		// Constructors
		TextFile(std::filesystem::path text_file_path);
		TextFile() = delete;

		// Overloaded [] operator to get access to each line of the contained
		// file
		auto operator[](const unsigned int index) const -> std::string;

		// Public Methods
		auto size() const -> unsigned int;
		auto valid() const -> bool;
		auto get_progress(const int current_line) const -> std::string;

		// Public Members
		unsigned int width; // Column Size of max

	private:

		// Private Members
		std::filesystem::path _text_fp;		// Filename
		std::vector<std::string> _contents; // Contents of text file
};

}