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

#include "compendium.hpp"

// Standard Constructor
Sorcery::Compendium::Compendium(System &system, Display &display, Graphics &graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Window and Graphics to Display
	_window = _display.window->get_window();

	// Get the Display Components
	_display.generate_components("compendium");
}

// Standard Destructor
Sorcery::Compendium::~Compendium() {

	_display.stop_background_movie();
}

auto Sorcery::Compendium::start() -> void {

	// Clear the window
	_window->clear();

	// Play the background movie!
	_display.fit_background_movie();
	_display.start_background_movie();

	_display.window->input_mode = WindowInputMode::DISPLAY_TEXT_FILE;

	// And do the main loop
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				return;

			if (_system.input->check_for_event(WindowInput::CANCEL, event)) {
				return;
			}
		}

		_window->clear();

		_display.start_background_movie();
		_display.update_background_movie();
		_display.draw_background_movie();

		_draw();
		_window->display();
	}
}

auto Sorcery::Compendium::stop() -> void {

	_display.stop_background_movie();
}

auto Sorcery::Compendium::_draw() -> void {

	_display.display_components("compendium");
	_display.display_cursor();
}
