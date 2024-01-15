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

#include "types/textfile.hpp"

// Standard Constructor
Sorcery::TextFile::TextFile(std::filesystem::path text_file_path) : _text_fp{text_file_path} {

	// Attempt to load the text file specified
	width = 0;
	if (std::ifstream file{CEESTR(text_file_path.string()), std::ifstream::in}; file.good()) {
		_contents.clear();
		_contents.push_back(EMPTY);
		auto line{""s};
		while (std::getline(file, line)) {

			// Get rid of trailing spaces and add to the buffer
			line = std::regex_replace(line, std::regex(" +$"), "");
			_contents.push_back(line);

			// Work out maximum length
			if (line.length() > width)
				width = line.length();
		}

		file.close();
	}
}

// Overload the array operator [] to allow direct access to the contents
auto Sorcery::TextFile::operator[](const unsigned int index) const -> std::string {

	try {
		return _contents.at(index);
	} catch (std::out_of_range &e) {
		return _contents.at(0);
	}
}

// Size (in lines)
auto Sorcery::TextFile::TextFile::size() const -> unsigned int {

	return _contents.size();
}

auto Sorcery::TextFile::get_progress(const int current_line) const -> std::string {

	// Work out progress through file
	const auto percent{(static_cast<float>(current_line) / static_cast<float>(_contents.size())) * 100};

	// Build status line
	return fmt::format("{}/{} ({}%)", current_line, _contents.size(), std::lround(percent));
}
