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

#include "frontend/mainmenu.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "frontend/compendium.hpp"
#include "frontend/license.hpp"
#include "frontend/options.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"

Sorcery::MainMenu::MainMenu(System *system, Display *display, UI *ui,
							Controller *controller)
	: _system{system},
	  _display{display},
	  _ui{ui},
	  _controller{controller} {

	_options = std::make_unique<Options>(_system, _display, _ui, _controller);
	_license = std::make_unique<License>(_system, _display, _ui, _controller);
	_compendium =
		std::make_unique<Compendium>(_system, _display, _ui, _controller);

	_initialise();
};

auto Sorcery::MainMenu::_initialise() -> bool {

	return true;
}

auto Sorcery::MainMenu::start() -> int {

	// Clear all controller and flow flags
	_controller->initialise("main_menu");

	// Start relevant animation worker threads
	_system->animation->refresh_attract();
	_system->animation->start_attract_th();

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			// Check for Quit Events
			ImGui_ImplSDL2_ProcessEvent(&event);
			done = _controller->check_for_abort(event);

			// Check for Back Event
			_controller->check_for_resize(event, _ui);
			_controller->check_for_back(event, _ui->dialog_exit->show);
		}

		if (!done) {

			_ui->display("main_menu");

			// Check for the results of a Popup Dialog
			if (_controller->has_flag("want_exit_game"))
				return MAIN_MENU_EXIT_GAME;
			else if (_controller->has_flag("want_new_game"))
				return MAIN_MENU_NEW_GAME;
			else if (_controller->has_flag("want_abort"))
				return ABORT_GAME;
			else if (_controller->has_flag("want_continue_game"))
				return MAIN_MENU_CONTINUE_GAME;

			// Check for the results of something being selected from a menu
			if (_controller->has_flag("show_compendium")) {
				_compendium->start();
				_compendium->stop();
			} else if (_controller->has_flag("show_options")) {
				_options->start(false);
				_options->stop();
			} else if (_controller->has_flag("show_license")) {
				_license->start();
				_license->stop();
			}
		}
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::MainMenu::stop() -> int {

	return 0;
}