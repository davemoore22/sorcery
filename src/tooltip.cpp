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
Sorcery::Tooltip::Tooltip (System& system, Display& display, Graphics& graphics):
	 _system {system}, _display {display}, _graphics {graphics} {

	// Get the Window and Graphics to Display
	_window = _display.window->get_window();

	// Get the standard layout information
	_layout = Component((*_display.layout)["global:tooltip"]);

	valid = false;
}

auto Sorcery::Tooltip::set(std::string& string) -> void {

	_strings.clear();
	_texts.clear();
	_width = 0;
	_height = 0;
	_frame_texture = sf::Texture();
	_frame = sf::Sprite();


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

	int x {18};
	int y {0};
	for (const auto each_string: _strings) {
		sf::Text text;
		text.setFont(_system.resources->fonts[_layout.font]);
		text.setCharacterSize(_layout.size);
		text.setFillColor(sf::Color(_layout.colour));
		text.setString(each_string);
		text.setPosition(x, 18 + y * 24);
		++y;
		_texts.push_back(text);
	}

	// Workout the size of the Frame needed
	size_t max_length {0};
	for (const auto& each_string: _strings) {
		if (each_string.length() > max_length) {
			max_length = each_string.length();
		}
	}
	_width = max_length + 4;
	_height = _strings.size() + 4;

	// Get the frame
	sf::RenderTexture frame_render_texture;
	_frame_texture = sf::Texture();
	_frame = sf::Sprite(_display.window->get_hint_frame(frame_render_texture, _frame_texture, _width / 2.15f,
		_height / 2.0f, _layout.alpha));
	// Divided here since 20 font size is much less than 18 unit measurement used for frame

	// need to use get character size to sort this out, remembering that gui units is 18 pixels

	// Get the background
	// _background = sf::RectangleShape(sf::Vector2f((_width * 18) - 18, (_height * 18) - 18));
	// _background.setFillColor(sf::Color(0, 0, 0, _layout.alpha));
	// _background.setPosition(9, 9);

	valid = true;
}

// Standard Destructor
Sorcery::Tooltip::~Tooltip() {
}

auto Sorcery::Tooltip::draw(sf::RenderTarget& target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	target.draw(_frame, states);
	// target.draw(_background);
	for (auto each_text: _texts) {
		target.draw(each_text, states);
	}
}

/*

auto tooltip = std::make_unique<Tooltip>(_system, _display, _graphics, hint);
	tooltips.push_back(std::move(tooltip));

	std::vector<std::unique_ptr<Tooltip>> ::iterator working_tooltips {tooltips.begin()};


if (!(*working_tooltips)->entry_time)
				(*working_tooltips)->entry_time = std::chrono::steady_clock::now();

			std::vector<std::unique_ptr<Tooltip>> tooltips;

 else
			(*working_tooltips)->entry_time = std::nullopt;


			*/