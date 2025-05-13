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
#include "core/controller.hpp"
#include "core/display.hpp"
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

Sorcery::Tavern::Tavern(System *system, Display *display, UI *ui,
						Controller *controller)
	: _system{system},
	  _display{display},
	  _ui{ui},
	  _controller{controller} {

	_add = std::make_unique<Add>(_system, _display, _ui, _controller);
	_remove = std::make_unique<Remove>(_system, _display, _ui, _controller);
	_reorder = std::make_unique<Reorder>(_system, _display, _ui, _controller);
	_inspect = std::make_unique<Inspect>(_system, _display, _ui, _controller);

	_initialise();
};

auto Sorcery::Tavern::_initialise() -> bool {

	_controller->selected["party_panel_selected"] = 0;

	return true;
}

auto Sorcery::Tavern::start(Game *game) -> int {

	_controller->initialise("tavern");
	_controller->set_flag("show_tavern");

	// Need this before accessing modal_inspect!
	_ui->create_dynamic_modal(game, "modal_inspect");
	_ui->create_dynamic_modal(game, "modal_identify");
	_ui->modal_inspect->show = false;
	_controller->clear_character("inspect");

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
			if (_controller->check_for_back(event))
				return BACK_TO_CASTLE;
		}

		_ui->display("tavern", game);

		if (!_controller->has_flag("show_tavern") &&
			_controller->has_flag("show_castle"))
			return BACK_TO_CASTLE;

		// Check for the results of something being selected from a menu
		if (_controller->has_flag("show_remove")) {
			_remove->start(game);
			_remove->stop(game);
			_controller->set_flag("show_tavern");
		} else if (_controller->has_flag("show_add")) {
			_add->start(game);
			_add->stop(game);
			_controller->set_flag("show_tavern");
		} else if (_controller->has_flag("show_reorder")) {
			_reorder->start(game, REORDER_MODE_TAVERN);
			_reorder->stop(game, REORDER_MODE_TAVERN);
			_controller->set_flag("show_tavern");
		} else if (_controller->has_character("inspect")) {
			_inspect->start(game, INSPECT_MODE_TAVERN,
							_controller->get_character("inspect"));
			_inspect->stop(game, INSPECT_MODE_TAVERN);
			_controller->set_flag("show_tavern");
			_controller->clear_character("inspect");
		}
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Tavern::stop() -> int {

	_controller->unset_flag("show_tavern");

	return 0;
}