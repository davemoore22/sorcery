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

#include "modules/castle.hpp"
#include "common/macro.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "modules/add.hpp"
#include "modules/choose.hpp"
#include "modules/inspect.hpp"
#include "modules/recovery.hpp"
#include "modules/remove.hpp"
#include "modules/reorder.hpp"
#include "modules/roster.hpp"
#include "modules/stay.hpp"
#include "modules/tavern.hpp"
#include "types/game.hpp"

Sorcery::Castle::Castle(Context &ctx)
	: _ctx{ctx} {

	_tavern = std::make_unique<Tavern>(_ctx);
	_inn = std::make_unique<Inn>(_ctx.system, _ctx.display, _ctx.ui,
								 _ctx.controller);
	_shop = std::make_unique<Shop>(_ctx.system, _ctx.display, _ctx.ui,
								   _ctx.controller);
	_temple = std::make_unique<Temple>(_ctx.system, _ctx.display, _ctx.ui,
									   _ctx.controller);

	_initialise();
};

auto Sorcery::Castle::_initialise() -> bool {

	_ctx.controller->set_selected("party_panel_selected", 0);

	return true;
}

auto Sorcery::Castle::start() -> int {

	_ctx.controller->initialise("castle");
	_ctx.controller->set_flag("show_castle");

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
			_ctx.controller->check_for_back(event, _ctx.ui->dialog_leave->show);

			// Check for Debug
			_ctx.controller->check_for_debug(event);
		}

		_ctx.ui->display("castle", _ctx.game);

		if (_ctx.controller->has_flag("want_leave_game")) {
			_ctx.game->move_party_to_tavern();
			_ctx.game->save_game();
			_ctx.controller->set_game(nullptr);
			return LEAVE_GAME;
		} else if (_ctx.controller->has_flag("want_abort"))
			return ABORT_GAME;
		else if (!_ctx.controller->has_flag("show_castle") &&
				 _ctx.controller->has_flag("show_edge_of_town"))
			return CASTLE_GO_TO_EDGE_OF_TOWN;

		// Check for the results of something being selected from a menu
		if (_ctx.controller->has_flag("show_tavern")) {
			_tavern->start();
			_tavern->stop();
		} else if (_ctx.controller->has_flag("show_inn")) {
			_inn->start(_ctx.game);
			_inn->stop();
		} else if (_ctx.controller->has_flag("show_shop")) {
			_shop->start(_ctx.game);
			_shop->stop();
		} else if (_ctx.controller->has_flag("show_temple")) {
			_temple->start(_ctx.game);
			_temple->stop();
		}
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Castle::stop() -> int {

	_ctx.controller->unset_flag("show_castle");

	return 0;
}