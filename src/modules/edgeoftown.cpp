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

#include "modules/edgeoftown.hpp"
#include "common/macro.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "modules/restart.hpp"
#include "modules/training.hpp"
#include "types/game.hpp"

Sorcery::EdgeOfTown::EdgeOfTown(System *system, Display *display, UI *ui,
								Controller *controller)
	: _system{system},
	  _display{display},
	  _ui{ui},
	  _controller{controller} {

	_initialise();

	_training_grounds =
		std::make_unique<Training>(_system, _display, _ui, _controller);
	_restart = std::make_unique<Restart>(_system, _display, _ui, _controller);
};

auto Sorcery::EdgeOfTown::_initialise() -> bool {

	_controller->selected["party_panel_selected"] = 0;

	return true;
}

auto Sorcery::EdgeOfTown::start(Game *game) -> int {

	_controller->initialise("edge_of_town");
	_controller->set_flag("show_edge_of_town");

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			// Check for Quit Events
			ImGui_ImplSDL2_ProcessEvent(&event);
			done = _controller->check_for_abort(event);

			// Check for Window Resize
			_controller->check_for_resize(event, _ui);

			// Check for Back Event
			_controller->check_for_back(event, _ui->dialog_leave->show);
		}

		_ui->display("edge_of_town", game);

		if (_controller->has_flag("want_leave_game")) {
			game->move_party_to_tavern();
			game->save_game();
			_controller->set_game(nullptr);
			return LEAVE_GAME;
		} else if (_controller->has_flag("want_abort"))
			return ABORT_GAME;
		else if (!_controller->has_flag("show_edge_of_town") &&
				 _controller->has_flag("show_castle"))
			return EDGE_OF_TOWN_GO_TO_CASTLE;

		// Check for the results of something being selected from a menu
		if (_controller->has_flag("show_training_grounds")) {
			game->move_party_to_tavern();
			game->save_game();
			_training_grounds->start(game);
			_training_grounds->stop();
		} else if (_controller->has_flag("show_restart")) {
			_restart->start(game);
			_restart->stop(game);
			if (_controller->has_flag("want_restart_expedition"))
				return RESTART_MAZE;

		} else if (_controller->has_flag("want_enter_maze"))
			return GO_TO_MAZE;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::EdgeOfTown::stop() -> int {

	_controller->unset_flag("show_edge_of_town");

	return 0;
}