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

#include "gui/textpanel.hpp"

// Standard Constructor
Sorcery::TextPanel::TextPanel(System *system, Display *display, Graphics *graphics, Component &component)
	: _system{system}, _display{display}, _graphics{graphics}, _layout{component} {

	// Not valid until we call the set command
	valid = false;
}

// Set the Text
auto Sorcery::TextPanel::set_text(const std::string &string) -> void {

	_strings.clear();
	_texts.clear();
	_width = 0;
	_height = 0;

	// Wrap the display lines
	// auto chunk_size{_layout.w};
	// auto wrapped_text{WORDWRAP(string, chunk_size)};
	auto wrapped_text{string};

	// Split the display lines into a vector
	const std::regex regex{(R"([@]+)")};
	std::sregex_token_iterator it{wrapped_text.begin(), wrapped_text.end(), regex, -1};
	std::vector<std::string> split{it, {}};
	split.erase(
		std::remove_if(split.begin(), split.end(), [](std::string const &s) { return s.size() == 0; }), split.end());
	_strings = split;

	auto x{0};
	auto y{0};
	for (const auto &each_string : _strings) {
		sf::Text text{};
		if (_display->get_bold())
			text.setStyle(sf::Text::Bold);
		text.setFont(_system->resources->fonts[_layout.font]);
		text.setCharacterSize(_layout.size);
		text.setFillColor(sf::Color(std::stoull(_layout["text_colour"].value(), 0, 16)));
		text.setString(each_string);
		text.setPosition(std::stoi(_layout["offset_x"].value()) + x,
			std::stoi(_layout["offset_y"].value()) + y * std::stoi(_layout["spacing_y"].value()));
		++y;
		_texts.push_back(text);
	}

	// Workout the size of the panel
	size_t max_length{0};
	for (const auto &each_string : _strings) {
		if (each_string.length() > max_length) {
			max_length = each_string.length();
		}
	}
	_width = max_length + 4;
	_height = _strings.size() + 4;

	// We're ok to draw it now
	valid = true;
}

auto Sorcery::TextPanel::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();
	for (auto each_text : _texts)
		target.draw(each_text, states);
}
