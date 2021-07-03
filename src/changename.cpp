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
// be useful, but WITHOUT ANY WARRANTY; wiWthout even the implied warranty of
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

#include "changename.hpp"

// Standard Constructor
Sorcery::ChangeName::ChangeName(
	System *system, Display *display, Graphics *graphics, std::string old_name)
	: _system{system}, _display{display}, _graphics{graphics}, _old_name{old_name} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Create the On-Screen Keyboard
	_keyboard = std::make_shared<Keyboard>(system, display, graphics);

	// Setup the Name
	_new_name = _old_name;
}

// Standard Destructor
Sorcery::ChangeName::~ChangeName() {}

auto Sorcery::ChangeName::start() -> std::optional<std::string> {

	_keyboard->setPosition((*_display->layout)["change_name:keyboard"].x,
		(*_display->layout)["change_name:keyboard"].y);

	const Component name_c{(*_display->layout)["change_name:name_candidate"]};

	_display->generate_components("change_name");
	_display->set_input_mode(WindowInputMode::INPUT_NAME);

	// Clear the window
	_window->clear();

	// Play the background movie!
	_display->fit_background_movie();
	_display->start_background_movie();

	while (_window->isOpen()) {
		sf::Event event{};
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				_window->close();

			// Handle enabling help overlay
			if (_system->input->check_for_event(WindowInput::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			if (_system->input->check_for_event(WindowInput::CANCEL, event))
				return std::nullopt;

			if (_system->input->check_for_event(WindowInput::BACK, event))
				return std::nullopt;
		}

		_window->clear();

		// Update Background Movie
		_display->start_background_movie();
		_display->update_background_movie();
		_display->draw_background_movie();

		_draw();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::ChangeName::stop() -> void {

	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);

	// Stop the background movie!
	_display->stop_background_movie();
}

auto Sorcery::ChangeName::get_new_name() -> std::string {

	return _new_name;
}

auto Sorcery::ChangeName::is_changed() -> bool {

	return _old_name != _new_name;
}

auto Sorcery::ChangeName::_draw() -> void {

	// Display Components
	_display->display_components("change_name");

	double lerp{_graphics->animation->colour_lerp};
	std::string display_name{">" + _new_name + "_"};
	sf::Text name_text;
	_display->window->draw_text(
		name_text, (*_display->layout)["change_name:name_candidate"], display_name, lerp);

	// Draw the On Screen Keyboard
	_keyboard->set_selected_background();
	_window->draw(*_keyboard);

	// And finally the Cursor
	_display->display_overlay();
	_display->display_cursor();
}
