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

#include "modules/sell.hpp"
#include "backends/imgui_impl_sdl2.h"		// for SDL_Event
#include "core/context.hpp"					// for Context
#include "core/controller/controller.hpp"	// for Controller
#include "core/controller/inputhandler.hpp" // For ControllerInputHandler
#include "core/enum.hpp"					// for Screen
#include "display/ui/popupmanager.hpp"		// for PopupManager
#include "display/ui/ui.hpp"				// for UI
#include "drawables/define.hpp"				// for BACK_TO_STORE, ABORT_GAME
#include <SDL_events.h>						// for SDL_PollEvent
#include <any>								// for any
#include <string>							// for basic_string

Sorcery::Sell::Sell(Context &ctx)
	: Module{ctx} {

	_initialise();
};

Sorcery::Sell::~Sell() {}

auto Sorcery::Sell::_initialise() -> bool {

	_ctx.controller->set_selected("sell_selected", -1);

	return true;
}

auto Sorcery::Sell::start() -> int {

	// Unlike what happens in the start() methods in other modules, we don't
	// call controller->initialise() here, as this module requires we know what
	// character we have selected to stay at the inn!
	_ctx.controller->go_to(Enums::Screen::SELL);

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
				return abort();

			case ModuleEvent::QUICKLOAD:
				continue;

			case ModuleEvent::NONE:
				break;
			}

			if (_ctx.controller->input->back(event)) {

				if (_ctx.ui->popup_manager->active())
					_ctx.ui->popup_manager->close();

				return BACK_TO_STORE;
			}
		}

		// Check for Buy Selected (remember +1 to selection)
		if (_ctx.controller->get_selected("sell_selected") > -1) {

			// Work out if we can sell the item (and if we can, do it!)

			// return BACK_TO_INN;
		}

		_ctx.ui->display_screen(Enums::Screen::SELL, _ctx.game);
		_ctx.tick();

		if (!_ctx.controller->wants(Enums::Screen::SELL) &&
			_ctx.controller->wants(Enums::Screen::STORE))
			return BACK_TO_STORE;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Sell::stop() -> int {

	_ctx.controller->set_selected("sell_selected", -1);
	_ctx.controller->go_to(Enums::Screen::STORE);

	return 0;
}