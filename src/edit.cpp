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

#include "edit.hpp"

// Standard Constructor
Sorcery::Edit::Edit(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();
	_current_character = std::nullopt;

	_character_panel = std::make_unique<CharPanel>(_system, _display, _graphics);
}

// Standard Destructor
Sorcery::Edit::~Edit() {}

auto Sorcery::Edit::start() -> std::optional<MenuItem> {

	return std::nullopt;
}

auto Sorcery::Edit::stop() -> void {

	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);

	// Stop the background movie!
	_display->stop_background_movie();
}

auto Sorcery::Edit::_draw() -> void {}
