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

#include "tooltip.hpp"

// Standard Constructor
Sorcery::Tooltip::Tooltip (System& system, Display& display, Graphics& graphics, std::string& string):
	 _system {system}, _display {display}, _graphics {graphics}, _string {string} {

	// Get the Window and Graphics to Display
	_window = _display.window->get_window();

	// Get the standard layout information
	_layout = Component((*_display.layout)["global:tooltip"]);

	// Workout the size of the Frame needed
	_width = string.length() < 40 ? string.length() : 40;
	_height = std::ceil(static_cast<float>(string.length()) / 40.0f);

	sf::RenderTexture frame_render_texture;
	_frame_texture = sf::Texture();

	// Get the frame
	_frame = sf::Sprite(_display.window->get_hint_frame(frame_render_texture, _frame_texture, _width + 4, _height + 4,
		_layout.alpha));

	// Get the display lines
	const std::regex regex(R"([#]+)");
	std::sregex_token_iterator it {string.begin(), string.end(), regex, -1};
	std::vector<std::string> split{it, {}};
	split.erase(std::remove_if(split.begin(), split.end(),
		[](std::string const& s) {
			return s.size() == 0;
		}),
    	split.end());
	_strings = split;

	_texts.clear();
	int x {18};
	int y {18};
	for (auto each_string: _strings) {
		sf::Text text;
		text.setFont(_system.resources->fonts[_layout.font]);
		text.setCharacterSize(_layout.size);
		text.setFillColor(sf::Color(_layout.colour));
		text.setString(each_string);
		text.setPosition(x, y * 18);
		++x;
		_texts.push_back(text);
	}
}

// Standard Destructor
Sorcery::Tooltip::~Tooltip() {
}

auto Sorcery::Tooltip::draw(sf::RenderTarget& target, sf::RenderStates states) const -> void {
	target.draw(_frame, states);
	for (auto each_text: _texts) {
		target.draw(each_text, states);
	}
}