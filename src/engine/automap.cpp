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

#include "engine/automap.hpp"
#include "core/context.hpp"				  // for Context
#include "core/controller/controller.hpp" // for Controller
#include "core/controller/inputhandler.hpp"
#include "core/enum.hpp"			   // for Screen
#include "display/ui/popupmanager.hpp" // for PopupManager
#include "display/ui/ui.hpp"		   // for UI
#include "drawables/define.hpp"		   // for BACK_FROM_AUTOMAP, ABORT_GAME
#include <SDL_events.h>				   // for SDL_Event, SDL_PollEvent
#include <any>						   // for any

Sorcery::Automap::Automap(Context &ctx)
	: Module{ctx} {

	_initialise();
};

Sorcery::Automap::~Automap() {}

auto Sorcery::Automap::_initialise() -> bool {

	return true;
}

auto Sorcery::Automap::start(void) -> int {

	_ctx.controller->go_to(Enums::Screen::AUTOMAP);
	_ctx.controller->initialise();

	fade_in(Enums::Screen::AUTOMAP, QUICK_FADE);

	// Main loop
	while (true) {

		SDL_Event event{};
		while (SDL_PollEvent(&event)) {

			switch (process_event(
				event,
				{.menu_key = true, .quicksave = false, .quickload = false})) {

			case ModuleEvent::ABORT:
				return abort();
				break;

			case ModuleEvent::QUICKLOAD:
				continue;

			case ModuleEvent::NONE:
				break;
			}

			if (_ctx.controller->input->back(event)) {

				if (_ctx.ui->popup_manager->active())
					_ctx.ui->popup_manager->close();

				return BACK_FROM_AUTOMAP;
			}
		}

		_ctx.ui->display_screen(Enums::Screen::AUTOMAP, _ctx.game);
		_ctx.tick();

		if (!_ctx.controller->wants(Enums::Screen::AUTOMAP))
			return BACK_FROM_AUTOMAP;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Automap::stop(void) -> void {

	fade_out(Enums::Screen::AUTOMAP, QUICK_FADE);
}
