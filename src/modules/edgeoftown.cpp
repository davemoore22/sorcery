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

#include "modules/edgeoftown.hpp"
#include "common/macro.hpp"
#include "core/application.hpp"
#include "core/audioplayer.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/define.hpp"
#include "core/display.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "modules/restart.hpp"
#include "modules/training.hpp"
#include "types/game.hpp"

Sorcery::EdgeOfTown::EdgeOfTown(Context &ctx)
	: _ctx{ctx} {

	_initialise();

	_training_grounds = std::make_unique<Training>(_ctx);
	_restart = std::make_unique<Restart>(_ctx);
};

Sorcery::EdgeOfTown::~EdgeOfTown() {}

auto Sorcery::EdgeOfTown::_initialise() -> bool {

	_ctx.controller->set_selected("party_panel_selected", 0);

	return true;
}

auto Sorcery::EdgeOfTown::start(const int mode) -> int {

	_ctx.controller->go_to(Enums::Screen::EDGEOFTOWN);
	_ctx.controller->initialise();

	_ctx.audio->set_volume(1.0f);

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

			if (_ctx.controller->check_for_quicksave(event))
				_ctx.application->save_state_to_binary(SAVE_STATE_FILENAME);
			else if (_ctx.controller->check_for_quickload(event))
				_ctx.application->load_state_from_binary(SAVE_STATE_FILENAME);
		}

		_ctx.ui->display(Enums::Screen::EDGEOFTOWN, _ctx.game);
		_ctx.tick();

		if (_ctx.controller->want_to_leave_game()) {
			_ctx.game->move_party_to_tavern();
			_ctx.game->save_game();
			_ctx.controller->set_game(nullptr);
			return LEAVE_GAME;
		} else if (_ctx.controller->want_to_abort())
			return ABORT_GAME;
		else if (!_ctx.controller->wants(Enums::Screen::EDGEOFTOWN) &&
				 _ctx.controller->wants(Enums::Screen::CASTLE))
			return EDGE_OF_TOWN_GO_TO_CASTLE;

		// Check for the results of something being selected from a menu
		if (_ctx.controller->wants(Enums::Screen::TRAINING)) {
			_ctx.game->move_party_to_tavern();
			_ctx.game->save_game();
			_training_grounds->start();
			_training_grounds->stop();
		} else if (_ctx.controller->wants(Enums::Screen::RESTART)) {
			_restart->start();
			_restart->stop();
			if (_ctx.controller->has_flag("want_restart_expedition"))
				return RESTART_MAZE;
		} else if (_ctx.controller->wants(Enums::Screen::ENGINE))
			return EDGE_OF_TOWN_GO_TO_MAZE;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::EdgeOfTown::stop() -> int {

	return 0;
}