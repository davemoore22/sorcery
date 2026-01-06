// Copyright (C) 2025 Dave Moore
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

#include "modules/tavern.hpp"
#include "common/macro.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "gui/modal.hpp"
#include "modules/add.hpp"
#include "modules/inspect.hpp"
#include "modules/remove.hpp"
#include "modules/reorder.hpp"
#include "modules/roster.hpp"
#include "types/game.hpp"
#include "types/state.hpp"

Sorcery::Tavern::Tavern(Context &ctx)
	: _ctx{ctx} {

	_add = std::make_unique<Add>(_ctx);
	_remove = std::make_unique<Remove>(_ctx);
	_reorder = std::make_unique<Reorder>(_ctx);
	_inspect = std::make_unique<Inspect>(_ctx);

	_initialise();
};

auto Sorcery::Tavern::_initialise() -> bool {

	_ctx.controller->set_selected("party_panel_selected", 0);

	return true;
}

auto Sorcery::Tavern::start() -> int {

	_ctx.controller->go_to(Enums::Screen::TAVERN);
	_ctx.controller->initialise();

	// Need this before accessing modal_inspect!
	_ctx.ui->create_dynamic_modal("modal_inspect");
	_ctx.ui->create_dynamic_modal("modal_identify");
	_ctx.ui->create_dynamic_modal("modal_drop");
	_ctx.ui->create_dynamic_modal("modal_trade");
	_ctx.ui->create_dynamic_modal("modal_use");
	_ctx.ui->create_dynamic_modal("modal_invoke");
	_ctx.ui->modal_inspect->show = false;
	_ctx.ui->modal_drop->show = false;
	_ctx.ui->modal_trade->show = false;
	_ctx.ui->modal_use->show = false;
	_ctx.ui->modal_trade->show = false;
	_ctx.ui->modal_invoke->show = false;
	_ctx.controller->clear_character("inspect");

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
				return BACK_TO_CASTLE;
		}

		_ctx.ui->display(Enums::Screen::TAVERN, _ctx.game);

		if (!_ctx.controller->wants(Enums::Screen::TAVERN) &&
			_ctx.controller->wants(Enums::Screen::CASTLE))
			return BACK_TO_CASTLE;

		// Check for the results of something being selected from a menu
		if (_ctx.controller->wants(Enums::Screen::REMOVE)) {
			_remove->start();
			_remove->stop();
			_ctx.controller->go_to(Enums::Screen::TAVERN);
		} else if (_ctx.controller->wants(Enums::Screen::ADD)) {
			_add->start();
			_add->stop();
			_ctx.controller->go_to(Enums::Screen::TAVERN);
		} else if (_ctx.controller->wants(Enums::Screen::REORDER)) {
			_reorder->start(REORDER_MODE_TAVERN);
			_reorder->stop(REORDER_MODE_TAVERN);
			_ctx.controller->go_to(Enums::Screen::TAVERN);
		} else if (_ctx.controller->has_character("inspect")) {
			_inspect->start(INSPECT_MODE_TAVERN,
							_ctx.controller->get_character("inspect"));
			_inspect->stop(INSPECT_MODE_TAVERN);
			_ctx.controller->go_to(Enums::Screen::TAVERN);
			_ctx.controller->clear_character("inspect");
		}
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Tavern::stop() -> int {

	return 0;
}