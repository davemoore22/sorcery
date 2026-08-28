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

#include "modules/choose.hpp"
#include "common/macro.hpp"
#include "core/application.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/define.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "display/display.hpp"
#include "display/ui/ui.hpp"
#include "drawables/define.hpp"
#include "drawables/dialog.hpp"
#include "resources/define.hpp"
#include "types/game.hpp"

Sorcery::Choose::Choose(Context &ctx)
	: Module{ctx} {

	_initialise();
};

auto Sorcery::Choose::_initialise() -> bool {

	_ctx.controller->clear_character(Enums::CharacterSlot::CHOOSE);

	return true;
}

auto Sorcery::Choose::start(const int mode) -> int {

	_ctx.controller->go_to(Enums::Screen::CHOOSE);
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
				return BACK_FROM_CHOOSE;
		}

		_ctx.ui->display_screen(Enums::Screen::CHOOSE, mode);
		_ctx.tick();

		if (!_ctx.controller->wants(Enums::Screen::CHOOSE))
			return BACK_FROM_CHOOSE;
		if (_ctx.controller->get_character(Enums::CharacterSlot::CHOOSE))
			return BACK_FROM_CHOOSE;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Choose::stop() -> int {

	return 0;
}