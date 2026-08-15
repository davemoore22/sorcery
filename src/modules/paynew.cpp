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

#include "modules/paynew.hpp"
#include "common/macro.hpp"
#include "core/application.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/define.hpp"
#include "core/display.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "gui/modal.hpp"
// #include "modules/healnew.hpp"
#include "resources/define.hpp"
#include "types/game.hpp"

Sorcery::PayNew::PayNew(Context &ctx)
	: Module{ctx} {

	//_heal = std::make_unique<HeakNew>(_ctx);

	_initialise();
};

auto Sorcery::PayNew::_initialise() -> bool {

	_ctx.controller->set_selected("party_panel_selected", 0);

	return true;
}

auto Sorcery::PayNew::start() -> int {

	_ctx.controller->go_to(Enums::Screen::PAY);
	_ctx.controller->clear_character(Enums::CharacterSlot::PAY);

	show_immediately();

	while (true) {

		SDL_Event event{};
		while (SDL_PollEvent(&event)) {

			switch (process_event(
				event,
				{.menu_key = true, .quicksave = false, .quickload = false})) {

			case ModuleEvent::ABORT:
				return ABORT_GAME;

			case ModuleEvent::QUICKLOAD:
				continue;

			case ModuleEvent::NONE:
				break;
			}

			if (_ctx.controller->check_for_back(event))
				return BACK_TO_TEMPLE;
		}

		_ctx.ui->display(Enums::Screen::PAY_NEW, _ctx.game);

		_ctx.tick();

		if (_ctx.controller->wants(Enums::Screen::TEMPLE))
			return BACK_TO_TEMPLE;

		if (_ctx.controller->has_character(Enums::CharacterSlot::PAY)) {

			// const auto result{_heal->start()};

			// if (result == ABORT_GAME)
			//	return ABORT_GAME;
			//
			//_heal->stop();

			return HEALED_OK;
		}
	}
}

auto Sorcery::PayNew::stop() -> int {

	_ctx.controller->clear_character(Enums::CharacterSlot::PAY);

	_ctx.controller->go_to(Enums::Screen::TEMPLE);

	return 0;
}