// Copyright (C) 2020 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#include "textfile.hpp"

// Standard Constructor
Sorcery::TextFile::TextFile(std::filesystem::path text_file_path) : _text_file_path{text_file_path} {

	// Attempt to load the text file specified
	width = 0;
	if (std::ifstream text_file{CSTR(text_file_path.string()), std::ifstream::in}; text_file.good()) {
		_contents_buffer.clear();
		_contents_buffer.push_back(EMPTY);
		std::string line_of_text{};
		while (std::getline(text_file, line_of_text)) {

			// Get rid of trailing spaces and add to the buffer
			line_of_text = std::regex_replace(line_of_text, std::regex(" +$"), "");
			_contents_buffer.push_back(line_of_text);

			// Work out maximum length
			if (line_of_text.length() > width)
				width = line_of_text.length();
		}

		text_file.close();
	}
}

// Overload the array operator [] to allow direct access to the contents
auto Sorcery::TextFile::operator[](const unsigned int index) -> std::string & {

	try {
		return _contents_buffer.at(index);
	} catch (std::out_of_range &e) {
		return _contents_buffer.at(0);
	}
}

// Size (in lines)
auto Sorcery::TextFile::TextFile::size() const -> unsigned int {

	return _contents_buffer.size();
}

auto Sorcery::TextFile::get_reading_progress(int current_line) const -> std::string {

	// Work out progress through file
	const float percent{(static_cast<float>(current_line) / static_cast<float>(_contents_buffer.size())) * 100};
	const float percentage{std::round(percent)};
	std::stringstream pss{};
	pss << std::fixed << std::setprecision(0) << percentage;
	const std::string progress{" (" + pss.str() + "%)"};

	// Build status line
	const std::string status_line{
		std::to_string(current_line) + "/" + std::to_string(_contents_buffer.size()) + progress};

	return status_line;
}
