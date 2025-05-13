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

#include "modules/choose.hpp"
#include "common/macro.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "types/game.hpp"

Sorcery::Choose::Choose(System *system, Display *display, UI *ui,
						Controller *controller)
	: _system{system},
	  _display{display},
	  _ui{ui},
	  _controller{controller} {

	_initialise();
};

auto Sorcery::Choose::_initialise() -> bool {

	_controller->clear_character("choose");

	return true;
}

auto Sorcery::Choose::start(Game *game, const int mode) -> int {

	_controller->initialise("choose");
	_controller->set_flag("show_choose");

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			// Check for Quit Events
			ImGui_ImplSDL2_ProcessEvent(&event);
			done = _controller->check_for_abort(event);

			// Check for Window Resize
			_controller->check_for_resize(event, _ui);

			// Check for Back Event
			if (_controller->check_for_back(event))
				return BACK_FROM_CHOOSE;
		}

		_ui->display("choose", game, mode);

		if (!_controller->has_flag("show_choose"))
			return BACK_FROM_CHOOSE;
		if (_controller->get_character("choose"))
			return BACK_FROM_CHOOSE;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Choose::stop() -> int {

	_controller->unset_flag("show_choose");

	return 0;
}