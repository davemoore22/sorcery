// Copyright (C) 2025 Dave Moore
//
// This file is part of Sorcery.
//
// Sorcery is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 2 of the License, or (at your option) any later
// version.
//
// Sorcery is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Sorcery.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "frontend/options.hpp"
#include "common/macro.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"

Sorcery::Options::Options(System *system, Display *display, UI *ui,
						  Controller *controller)
	: _system{system},
	  _display{display},
	  _ui{ui},
	  _controller{controller} {

	_initialise();
};

auto Sorcery::Options::_initialise() -> bool {

	_is_in_game = false;

	return true;
}

auto Sorcery::Options::start(const bool is_in_game) -> int {

	_is_in_game = is_in_game;
	_controller->initialise("options");
	_controller->set_flag("show_options");

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			// Check for Quit or Back Events
			ImGui_ImplSDL2_ProcessEvent(&event);
			_controller->check_for_resize(event, _ui);
			done = _controller->check_for_abort(event);
			if (_controller->check_for_back(event))
				return GO_TO_FRONT_END; // Or back to the Game
		}

		_ui->display("options");
		if (_controller->has_flag("want_abort"))
			return ABORT_GAME;
		else if (!_controller->has_flag("show_options"))
			return GO_TO_FRONT_END;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Options::stop() -> int {

	if (_is_in_game)
		_controller->move_screen("show_options", "show_engine");
	else
		_controller->move_screen("show_options", "show_main_menu");

	return 0;
}