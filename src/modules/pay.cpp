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

#include "modules/pay.hpp"
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
#include "gui/input.hpp"
#include "gui/modal.hpp"
#include "modules/heal.hpp"
#include "resources/define.hpp"
#include "types/game.hpp"

Sorcery::Pay::Pay(Context &ctx)
	: Module{ctx} {

	_heal = std::make_unique<Heal>(_ctx);

	_initialise();
};

Sorcery::Pay::~Pay() {}

auto Sorcery::Pay::_initialise() -> bool {

	_ctx.controller->set_selected("pay_selected", 0);

	return true;
}

auto Sorcery::Pay::start() -> int {

	// Don't initialise here
	_ctx.controller->go_to(Enums::Screen::PAY);

	show_immediately();

	// Need this before accessing dynamic modals!
	_ctx.controller->clear_character(Enums::CharacterSlot::PAY);
	_ctx.controller->unset_selected("pay_selected");

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event{};
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
				return BACK_TO_STAY;
		}

		_ctx.ui->display(Enums::Screen::PAY, _ctx.game);
		_ctx.tick();

		if (!_ctx.controller->wants(Enums::Screen::PAY) &&
			_ctx.controller->wants(Enums::Screen::TEMPLE))
			return HEALED_NOT;
		else if (_ctx.controller->has_selected("pay_selected")) {
			const auto result{_heal->start()};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_heal->stop();
			return HEALED_OK;
		}
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Pay::stop(const bool paid) -> int {

	if (paid) {
		_ctx.controller->go_to(Enums::Screen::RESULTS);
		_ctx.controller->clear_character(Enums::CharacterSlot::PAY);
	} else {
		_ctx.controller->go_to(Enums::Screen::TEMPLE);
		_ctx.controller->clear_character(Enums::CharacterSlot::PAY);
	}

	return 0;
}