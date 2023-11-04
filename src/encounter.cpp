// Copyright (C) 2023 Dave Moore
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

#include "encounter.hpp"

// Standard Constructor
Sorcery::Encounter::Encounter(System *system, Display *display, Graphics *graphics, Game *game, CombatType type)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game}, _type{type} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();
	_duration = DELAY_ENCOUNTER;
}

auto Sorcery::Encounter::start() -> void {

	_display->generate("encounter", _sprites, _texts, _frames);
	_display->window->save_screen();

	_start = std::chrono::system_clock::now();

	bool display{true};

	while (display) {

		_current_time = std::chrono::system_clock::now();

		const auto time_elapsed{_current_time.value() - _start.value()};
		const auto time_elapsed_msec{std::chrono::duration_cast<std::chrono::milliseconds>(time_elapsed)};
		if (time_elapsed_msec.count() > _duration)
			display = false;

		_window->clear();

		_draw();
		_window->display();
	}
};

auto Sorcery::Encounter::_draw() -> void {

	_display->window->restore_screen();

	_display->display("encounter", _sprites, _texts, _frames);

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
};
