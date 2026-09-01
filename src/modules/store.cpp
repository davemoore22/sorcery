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

#include "modules/store.hpp"
#include "backends/imgui_impl_sdl2.h"		// for SDL_Event
#include "core/context.hpp"					// for Context
#include "core/controller/controller.hpp"	// for Controller
#include "core/controller/inputhandler.hpp" // For ControllerInputHandler
#include "core/enum.hpp"					// for Screen, CharacterSlot
#include "display/ui/ui.hpp"				// for UI
#include "drawables/define.hpp"				// for ABORT_GAME, BACK_TO_CASTLE
#include "modules/buy.hpp"					// for Buy
#include "modules/identify.hpp"				// for Identify
#include "modules/sell.hpp"					// for Sell
#include "modules/uncurse.hpp"				// for Uncurse
#include <SDL_events.h>						// for SDL_PollEvent
#include <any>								// for any
#include <string>							// for basic_string

Sorcery::Store::Store(Context &ctx)
	: Module{ctx} {

	_initialise();

	_buy = std::make_unique<Buy>(_ctx);
	_sell = std::make_unique<Sell>(_ctx);
	_identify = std::make_unique<Identify>(_ctx);
	_uncurse = std::make_unique<Uncurse>(_ctx);
};

Sorcery::Store::~Store() {}

auto Sorcery::Store::_initialise() -> bool {

	_ctx.controller->set_selected("store_selected", -1);

	return true;
}

auto Sorcery::Store::start() -> int {

	// Unlike what happens in the start() methods in other modules, we don't
	// call controller->initialise() here, as this module requires we know what
	// character we have selected to stay at the inn!
	_ctx.controller->go_to(Enums::Screen::STORE);

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

			if (_ctx.controller->input->back(event))
				return BACK_TO_CASTLE;
		}

		// Check for the results of something being selected from a menu
		if (_ctx.controller->wants(Enums::Screen::BUY)) {
			const auto result{_buy->start()};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_buy->stop();
			_ctx.controller->go_to(Enums::Screen::STORE);
		} else if (_ctx.controller->wants(Enums::Screen::SELL)) {
			const auto result{_sell->start()};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_sell->stop();
			_ctx.controller->go_to(Enums::Screen::STORE);
		} else if (_ctx.controller->wants(Enums::Screen::IDENTIFY)) {
			const auto result{_identify->start()};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_identify->stop();
			_ctx.controller->go_to(Enums::Screen::STORE);
		} else if (_ctx.controller->wants(Enums::Screen::UNCURSE)) {
			const auto result{_uncurse->start()};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_uncurse->stop();
			_ctx.controller->go_to(Enums::Screen::STORE);
		}

		_ctx.ui->display_screen(Enums::Screen::STORE, _ctx.game);
		_ctx.tick();

		if (!_ctx.controller->wants(Enums::Screen::STORE) &&
			_ctx.controller->wants(Enums::Screen::SHOP))
			return BACK_TO_INN;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Store::stop() -> int {

	_ctx.controller->clear_character(Enums::CharacterSlot::STORE);
	_ctx.controller->go_to(Enums::Screen::SHOP);

	return 0;
}