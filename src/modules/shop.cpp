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

#include "modules/shop.hpp"
#include "common/macro.hpp"
#include "core/application.hpp"
#include "core/context.hpp"
#include "core/controller/controller.hpp"
#include "core/controller/inputhandler.hpp" // For ControllerInputHandler
#include "core/define.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "display/display.hpp"
#include "display/ui/popupmanager.hpp" // for PopupManager
#include "display/ui/popupstore.hpp"
#include "display/ui/ui.hpp"
#include "drawables/define.hpp"
#include "drawables/dialog.hpp"
#include "drawables/modal.hpp"
#include "modules/inspect.hpp"
#include "modules/store.hpp"
#include "resources/define.hpp"
#include "types/game.hpp"

Sorcery::Shop::Shop(Context &ctx)
	: Module{ctx} {

	_store = std::make_unique<Store>(_ctx);
	_inspect = std::make_unique<Inspect>(_ctx);

	_initialise();
};

auto Sorcery::Shop::_initialise() -> bool {

	_ctx.controller->set_selected("party_panel_selected", 0);

	return true;
}

auto Sorcery::Shop::start() -> int {

	_ctx.controller->go_to(Enums::Screen::SHOP);
	_ctx.controller->initialise();

	show_immediately();

	// Need this before accessing modal_inspect!
	_ctx.ui->popups->create_dynamic_modal("modal_inspect");
	_ctx.ui->popups->create_dynamic_modal("modal_trade");
	_ctx.ui->popups->create_dynamic_modal("modal_give");
	_ctx.ui->popups->modal_inspect->show = false;
	_ctx.ui->popups->modal_give->show = false;
	_ctx.ui->popups->modal_trade->show = false;

	_ctx.controller->clear_character(Enums::CharacterSlot::INSPECT);

	// Main loop
	while (true) {

		SDL_Event event{};
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

				return BACK_TO_CASTLE;
			}
		}

		_ctx.ui->display_screen(Enums::Screen::SHOP, _ctx.game);
		_ctx.tick();

		if (!_ctx.controller->wants(Enums::Screen::SHOP) &&
			_ctx.controller->wants(Enums::Screen::CASTLE))
			return BACK_TO_CASTLE;
		else if (_ctx.controller->has_character(Enums::CharacterSlot::STORE)) {
			const auto result{_store->start()};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_store->stop();
			_ctx.controller->clear_character(Enums::CharacterSlot::STORE);
		} else if (_ctx.controller->has_character(
					   Enums::CharacterSlot::INSPECT)) {
			const auto result{_inspect->start(
				INSPECT_MODE_BASE | INSPECT_MODE_ACTIONS,
				_ctx.controller->get_character(Enums::CharacterSlot::INSPECT))};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_inspect->stop(INSPECT_MODE_BASE | INSPECT_MODE_ACTIONS);
			_ctx.controller->clear_character(Enums::CharacterSlot::INSPECT);
		}
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Shop::stop() -> int {

	return 0;
}