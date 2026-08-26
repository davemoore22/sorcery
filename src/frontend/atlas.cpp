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

#include "frontend/atlas.hpp"
#include "common/sdl2.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/enum.hpp"
#include "display/ui.hpp"
#include "drawables/define.hpp"

Sorcery::Atlas::Atlas(Context &ctx)
	: Module{ctx} {

	_initialise();
};

auto Sorcery::Atlas::_initialise() -> bool {

	_ctx.controller->set_selected("atlas_selected", 0);

	return true;
}

auto Sorcery::Atlas::start() -> int {

	_ctx.controller->go_to(Enums::Screen::ATLAS);
	_ctx.controller->initialise();

	fade_in(Enums::Screen::ATLAS, QUICK_FADE);

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
				return GO_TO_COMPENDIUM;
		}

		if (_ctx.controller->want_to_abort())
			return ABORT_GAME;
		else if (!_ctx.controller->wants(Enums::Screen::ATLAS))
			return GO_TO_COMPENDIUM;

		_ctx.ui->display(Enums::Screen::ATLAS);

		_ctx.tick();
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Atlas::stop() -> int {

	_ctx.controller->go_to(Enums::Screen::COMPENDIUM);

	fade_out(Enums::Screen::ATLAS, QUICK_FADE);

	return 0;
}