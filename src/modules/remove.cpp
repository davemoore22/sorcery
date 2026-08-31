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

#include "modules/remove.hpp"
#include "backends/imgui_impl_sdl2.h"	  // for SDL_Event
#include "core/context.hpp"				  // for Context
#include "core/controller/controller.hpp" // for Controller
#include "core/enum.hpp"				  // for Screen
#include "display/ui/ui.hpp"			  // for UI
#include "drawables/define.hpp"			  // for BACK_TO_TAVERN, ABORT_GAME
#include "types/game.hpp"				  // for Game
#include <SDL_events.h>					  // for SDL_PollEvent
#include <any>							  // for any
#include <string>						  // for basic_string

Sorcery::Remove::Remove(Context &ctx)
	: Module{ctx} {

	_initialise();
};

auto Sorcery::Remove::_initialise() -> bool {

	_ctx.controller->set_selected("remove_selected", 0);

	return true;
}

auto Sorcery::Remove::start() -> int {

	_ctx.controller->go_to(Enums::Screen::REMOVE);
	_ctx.controller->initialise();

	show_immediately();

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			switch (process_event(
				event,
				{.menu_key = true, .quicksave = false, .quickload = false})) {

			case ModuleEvent::ABORT:
				done = true;
				break;

			case ModuleEvent::QUICKLOAD:
				continue;

			case ModuleEvent::NONE:
				break;
			}

			if (_ctx.controller->check_for_back(event))
				return BACK_TO_TAVERN;
		}

		_ctx.ui->display_screen(Enums::Screen::REMOVE, _ctx.game);
		_ctx.tick();

		if (!_ctx.controller->wants(Enums::Screen::REMOVE) &&
			_ctx.controller->wants(Enums::Screen::TAVERN))
			return BACK_TO_TAVERN;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Remove::stop() -> int {

	_ctx.game->save_game();

	return 0;
}