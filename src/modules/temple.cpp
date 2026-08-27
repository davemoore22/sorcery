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

#include "modules/temple.hpp"
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
#include "drawables/modal.hpp"
#include "modules/heal.hpp"
#include "modules/pay.hpp"
#include "resources/define.hpp"
#include "types/game.hpp"

Sorcery::Temple::Temple(Context &ctx)
	: Module{ctx} {

	_pay = std::make_unique<Pay>(_ctx);

	_initialise();
};

Sorcery::Temple::~Temple() = default;

auto Sorcery::Temple::_initialise() -> bool {

	_ctx.controller->set_selected("party_panel_selected", 0);

	return true;
}

auto Sorcery::Temple::start() -> int {

	_ctx.controller->initialise();
	_ctx.controller->go_to(Enums::Screen::TEMPLE);

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
				return BACK_TO_CASTLE;
		}

		_ctx.ui->display(Enums::Screen::TEMPLE, _ctx.game);

		_ctx.tick();

		if (_ctx.controller->wants(Enums::Screen::CASTLE))
			return BACK_TO_CASTLE;

		if (_ctx.controller->has_character(Enums::CharacterSlot::HELP)) {

			const auto result{_pay->start()};
			if (result == ABORT_GAME)
				return ABORT_GAME;

			_pay->stop();

			_ctx.controller->clear_character(Enums::CharacterSlot::HELP);
		}
	}
}

auto Sorcery::Temple::stop() -> int {

	return 0;
}