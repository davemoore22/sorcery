// Copyright (C) 2021 Dave Moore
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

#include "create.hpp"

// Standard Constructor
Sorcery::Create::Create(System &system, Display &display, Graphics &graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Window and Graphics to Display
	_window = _display.window->get_window();

	// Get the Display Components
	_display.generate_components("create");
}

// Standard Destructor
Sorcery::Create::~Create() {

	_display.stop_background_movie();
}

auto Sorcery::Create::start() -> std::optional<MenuItem> {

	const Component bg_c{(*_display.layout)["create:background"]};
	const sf::IntRect bg_rect(1147, 249, 773, 388);
	_bg.setTexture(_system.resources->textures[TOWN_TEXTURE]);
	_bg.setTextureRect(bg_rect);
	_bg.setScale(bg_c.scale, bg_c.scale);
	_bg.setPosition(_display.window->get_x(_bg, bg_c.x), _display.window->get_y(_bg, bg_c.y));

	// Clear the window
	_window->clear();

	// Play the background movie!
	_display.start_background_movie();

	// Will need to change this for the seven screens
	_display.window->input_mode = WindowInputMode::NORMAL;

	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				_window->close();

			if (_system.input->check_for_event(WindowInput::CANCEL, event))
				return std::nullopt;
		}

		_window->clear();

		_display.start_background_movie();
		_display.update_background_movie();
		_display.draw_background_movie();

		_draw();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::Create::stop() -> void {

	_display.stop_background_movie();
}

auto Sorcery::Create::_draw() -> void {

	// Display Components
	_display.display_components("training_grounds");

	// Custom Layering
	_window->draw(_bg);

	// And draw the current state of the character!

	// And finally the Cursor
	_display.display_cursor();
}
