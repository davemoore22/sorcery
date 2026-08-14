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

#include "modules/uncurse.hpp"
#include "common/macro.hpp"
#include "core/application.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "resources/define.hpp"
#include "types/game.hpp"

Sorcery::Uncurse::Uncurse(Context &ctx)
	: Module{ctx} {

	_initialise();
};

Sorcery::Uncurse::~Uncurse() {}

auto Sorcery::Uncurse::_initialise() -> bool {

	_ctx.controller->set_selected("uncurse_selected", -1);

	return true;
}

auto Sorcery::Uncurse::start() -> int {

	_ctx.controller->go_to(Enums::Screen::UNCURSE);

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
				return BACK_TO_STORE;

			// Check for Buy Selected (remember +1 to selection)
			if (_ctx.controller->get_selected("uncurse_selected") > -1) {

				// Work out if we can sell the item (and if we can, do it!)

				// return BACK_TO_INN;
			}
		}

		_ctx.ui->display(Enums::Screen::UNCURSE, _ctx.game);
		_ctx.tick();

		if (!_ctx.controller->wants(Enums::Screen::UNCURSE) &&
			_ctx.controller->wants(Enums::Screen::STORE))
			return BACK_TO_STORE;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Uncurse::stop() -> int {

	_ctx.controller->set_selected("uncurse_selected", -1);
	_ctx.controller->go_to(Enums::Screen::STORE);

	return 0;
}