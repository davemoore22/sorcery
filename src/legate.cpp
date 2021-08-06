// Copyright (C) 2021 Dave Moore
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

#include "legate.hpp"

// Standard Constructor
Sorcery::Legate::Legate(
	System *system, Display *display, Graphics *graphics, Character *character)
	: _system{system}, _display{display}, _graphics{graphics}, _character{
																   character} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();
}

// Standard Destructor
Sorcery::Legate::~Legate() {}

auto Sorcery::Legate::start() -> bool {

	_display->generate("legate");
	//_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);

	// Clear the window
	_window->clear();

	// Play the background movie!
	_display->fit_bg_movie();
	_display->start_bg_movie();

	while (_window->isOpen()) {
		sf::Event event{};
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				_window->close();

			// Handle enabling help overlay
			if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			if (_system->input->check(WindowInput::CANCEL, event))
				return false;

			if (_system->input->check(WindowInput::BACK, event))
				return false;
		}

		_window->clear();

		// Update Background Movie
		_display->start_bg_movie();
		_display->update_bg_movie();
		_display->draw_bg_movie();

		_draw();
		_window->display();
	}

	return false;
}

auto Sorcery::Legate::stop() -> void {

	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);

	// Stop the background movie!
	_display->stop_bg_movie();
}

auto Sorcery::Legate::_draw() -> void {

	// Display Components
	_display->display("legate");

	/* // And the Menu
	_menu->generate((*_display->layout)["change_class:menu"]);
	const sf::Vector2f menu_pos((*_display->layout)["change_class:menu"].x,
		(*_display->layout)["change_class:menu"].y);
	_menu->setPosition(menu_pos);
	_window->draw(*_menu);

	if (_ip->valid) {
		_ip->setPosition((*_display->layout)["change_class:info_panel"].x,
			(*_display->layout)["change_class:info_panel"].y);
		_window->draw(*_ip);
	}

	if (_show_not_changed) {
		_not_changed->update();
		_window->draw(*_not_changed);
	}

	if (_show_confirm) {
		_confirm->update();
		_window->draw(*_confirm);
	} */

	// And finally the Cursor
	_display->display_overlay();
	_display->display_cursor();
}
