// Copyright (C) 2026 Dave Moore
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

#include "modules/restart.hpp"
#include "common/macro.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "types/game.hpp"

Sorcery::Restart::Restart(Context &ctx)
	: _ctx{ctx} {

	_initialise();
};

auto Sorcery::Restart::_initialise() -> bool {

	_ctx.controller->set_selected("restart_selected", 0);

	return true;
}

auto Sorcery::Restart::start() -> int {

	_ctx.controller->go_to(Enums::Screen::RESTART);
	_ctx.controller->initialise();

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			// Check for Quit Events
			ImGui_ImplSDL2_ProcessEvent(&event);
			done = _ctx.controller->check_for_abort(event);

			// Check for Window Resize
			_ctx.controller->check_for_resize(event, _ctx.ui);

			// Check for Back Event
			if (_ctx.controller->check_for_back(event))
				return BACK_FROM_ROSTER;
		}

		_ctx.ui->display(Enums::Screen::RESTART, _ctx.game);
		_ctx.tick();

		if (_ctx.controller->has_flag("want_restart_expedition")) {
			return RESTART_MAZE;
		} else if (!_ctx.controller->wants(Enums::Screen::RESTART) &&
				   _ctx.controller->wants(Enums::Screen::EDGEOFTOWN))
			return BACK_TO_EDGE_OF_TOWN;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Restart::stop() -> int {

	_ctx.game->save_game();
	_ctx.controller->go_to(Enums::Screen::EDGEOFTOWN);

	return 0;
}