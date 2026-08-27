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

#include "modules/castle.hpp"
#include "common/macro.hpp"
#include "core/application.hpp"
#include "core/audioplayer.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/define.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "display/display.hpp"
#include "display/ui/popupstore.hpp"
#include "display/ui/ui.hpp"
#include "drawables/define.hpp"
#include "drawables/dialog.hpp"
#include "drawables/modal.hpp"
#include "modules/add.hpp"
#include "modules/choose.hpp"
#include "modules/inn.hpp"
#include "modules/inspect.hpp"
#include "modules/pay.hpp"
#include "modules/recovery.hpp"
#include "modules/remove.hpp"
#include "modules/reorder.hpp"
#include "modules/shop.hpp"
#include "modules/stay.hpp"
#include "modules/store.hpp"
#include "modules/tavern.hpp"
#include "modules/temple.hpp"
#include "resources/define.hpp"
#include "types/game.hpp"

Sorcery::Castle::Castle(Context &ctx)
	: Module{ctx} {

	_tavern = std::make_unique<Tavern>(_ctx);
	_inn = std::make_unique<Inn>(_ctx);
	_shop = std::make_unique<Shop>(_ctx);
	_temple = std::make_unique<Temple>(_ctx);
	_inspect = std::make_unique<Inspect>(_ctx);

	_initialise();
};

Sorcery::Castle::~Castle() {}

auto Sorcery::Castle::_initialise() -> bool {

	_ctx.controller->set_selected("party_panel_selected", 0);

	return true;
}

auto Sorcery::Castle::start() -> int {

	_ctx.controller->go_to(Enums::Screen::CASTLE);
	_ctx.controller->initialise();

	fade_in(Enums::Screen::CASTLE, QUICK_FADE);

	// Need this before accessing modal_inspect!
	_ctx.ui->popups->create_dynamic_modal("modal_inspect");
	_ctx.ui->popups->create_dynamic_modal("modal_identify");
	_ctx.ui->popups->create_dynamic_modal("modal_equip");
	_ctx.ui->popups->create_dynamic_modal("modal_remove");
	_ctx.ui->popups->create_dynamic_modal("modal_spell");
	_ctx.ui->popups->create_dynamic_modal("modal_drop");
	_ctx.ui->popups->create_dynamic_modal("modal_trade");
	_ctx.ui->popups->create_dynamic_modal("modal_use");
	_ctx.ui->popups->create_dynamic_modal("modal_give");
	_ctx.ui->popups->create_dynamic_modal("modal_invoke");
	_ctx.ui->popups->modal_inspect->show = false;
	_ctx.ui->popups->modal_identify->show = false;
	_ctx.ui->popups->modal_equip->show = false;
	_ctx.ui->popups->modal_remove->show = false;
	_ctx.ui->popups->modal_spell->show = false;
	_ctx.ui->popups->modal_drop->show = false;
	_ctx.ui->popups->modal_give->show = false;
	_ctx.ui->popups->modal_trade->show = false;
	_ctx.ui->popups->modal_use->show = false;
	_ctx.ui->popups->modal_invoke->show = false;

	_ctx.controller->clear_character(Enums::CharacterSlot::INSPECT);

	_ctx.audio->set_volume(1.0f);

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event{};
		while (SDL_PollEvent(&event)) {

			switch (process_event(event, {.menu_key = true, .debug = true})) {

			case ModuleEvent::ABORT:
				done = true;
				break;

			case ModuleEvent::QUICKLOAD:
				continue;

			case ModuleEvent::NONE:
				break;
			}

			_ctx.controller->check_for_back(
				event, _ctx.ui->popups->dialog_leave->show);
		}

		_ctx.ui->display(Enums::Screen::CASTLE, _ctx.game);
		_ctx.tick();

		if (_ctx.controller->want_to_leave_game()) {
			_ctx.game->move_party_to_tavern();
			_ctx.game->save_game();
			_ctx.controller->set_game(nullptr);
			return LEAVE_GAME;
		} else if (!_ctx.controller->wants(Enums::Screen::CASTLE) &&
				   _ctx.controller->wants(Enums::Screen::EDGEOFTOWN))
			return CASTLE_GO_TO_EDGE_OF_TOWN;

		// Check for the results of something being selected from a menu
		if (_ctx.controller->wants(Enums::Screen::TAVERN)) {
			const auto result{_tavern->start()};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_tavern->stop();
		} else if (_ctx.controller->wants(Enums::Screen::INN)) {
			const auto result{_inn->start()};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_inn->stop();
		} else if (_ctx.controller->wants(Enums::Screen::SHOP)) {
			const auto result{_shop->start()};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_shop->stop();
		} else if (_ctx.controller->wants(Enums::Screen::TEMPLE)) {
			const auto result{_temple->start()};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_temple->stop();
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

auto Sorcery::Castle::stop() -> int {

	fade_out(Enums::Screen::CASTLE, QUICK_FADE);

	return 0;
}