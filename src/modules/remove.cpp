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

#include "modules/remove.hpp"
#include "common/macro.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "types/game.hpp"

Sorcery::Remove::Remove(System *system, Display *display, UI *ui,
						Controller *controller)
	: _system{system},
	  _display{display},
	  _ui{ui},
	  _controller{controller} {

	_initialise();
};

auto Sorcery::Remove::_initialise() -> bool {

	_controller->selected["remove_selected"] = 0;

	return true;
}

auto Sorcery::Remove::start(Game *game) -> int {

	_controller->initialise("remove");
	_controller->set_flag("show_remove");

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
				return BACK_FROM_ROSTER;
		}

		_ui->display("remove", game);

		if (!_controller->has_flag("show_remove") &&
			_controller->has_flag("show_tavern"))
			return BACK_TO_TAVERN;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Remove::stop(Game *game) -> int {

	game->save_game();
	_controller->unset_flag("show_remove");

	return 0;
}