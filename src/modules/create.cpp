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

#include "modules/create.hpp"
#include "common/macro.hpp"
#include "core/application.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "gui/input.hpp"
#include "types/character.hpp"
#include "types/game.hpp"

Sorcery::Create::Create(Application *application, System *system,
						Display *display, UI *ui, Controller *controller)
	: _application{application},
	  _system{system},
	  _display{display},
	  _ui{ui},
	  _controller{controller} {

	_initialise();
};

Sorcery::Create::~Create() {};

auto Sorcery::Create::_initialise() -> bool {

	_stage = Enums::Character::Stage::ENTER_NAME;

	return true;
}

auto Sorcery::Create::start(Game *game) -> int {

	_controller->initialise("create");
	_controller->set_flag("show_create");
	_controller->set_flag("want_enter_name");
	_controller->unset_flag("want_choose_race");
	_controller->unset_flag("want_choose_alignment");
	_controller->set_method(Enums::Character::Method::NO_METHOD);

	std::shared_ptr<Character> candidate =
		std::make_shared<Character>(_system, _application->get_resources());

	_controller->inject_character(candidate);
	candidate->reset(Enums::Character::Stage::ENTER_NAME);

	_ui->first_frame = true;

	// Main loop
	auto done{false};
	SDL_Event event;
	while (!done) {

		while (SDL_PollEvent(&event)) {

			// Check for Quit Events
			ImGui_ImplSDL2_ProcessEvent(&event);
			done = _controller->check_for_abort(event);

			// Check for Window Resize
			_controller->check_for_resize(event, _ui);

			// Check for Back Event
			if (_controller->check_for_back(event)) {
				return BACK_TO_CHOOSE_METHOD;
			}
		}

		_ui->display("create_name", game, static_cast<int>(_stage));

		if (!_controller->has_flag("show_create") &&
			_controller->has_flag("show_method")) {
			return BACK_TO_CHOOSE_METHOD;
		} else if (!_controller->has_flag("show_create") &&
				   _controller->has_flag("show_training_grounds")) {
			return BACK_TO_TRAINING_GROUNDS;
		}

		// Check to see if we have finished entering the name (the stage and
		// name are set in controller->handle_input_button_click())
		if (!_controller->has_flag("want_enter_name") &&
			candidate->get_stage() != Enums::Character::Stage::ENTER_NAME) {

			while (!done) {

				// Move to the next stage
				while (SDL_PollEvent(&event)) {

					// Check for Quit Events
					ImGui_ImplSDL2_ProcessEvent(&event);
					done = _controller->check_for_abort(event);

					// Check for Window Resize
					_controller->check_for_resize(event, _ui);

					// Check for Back Event
					if (_controller->check_for_back(event)) {
						return BACK_TO_CHOOSE_METHOD;
					}
				}

				_ui->display("create_race", game, static_cast<int>(_stage));

				if (!_controller->has_flag("show_create") &&
					_controller->has_flag("show_method")) {
					return BACK_TO_CHOOSE_METHOD;
				} else if (!_controller->has_flag("show_create") &&
						   _controller->has_flag("show_training_grounds")) {
					return BACK_TO_TRAINING_GROUNDS;
				}

				if (!_controller->has_flag("want_choose_race") &&
					_controller->has_flag("want_choose_alignment")) {

					while (!done) {

						// Move to the next stage
						while (SDL_PollEvent(&event)) {

							// Check for Quit Events
							ImGui_ImplSDL2_ProcessEvent(&event);
							done = _controller->check_for_abort(event);

							// Check for Window Resize
							_controller->check_for_resize(event, _ui);

							// Check for Back Event
							if (_controller->check_for_back(event)) {
								return BACK_TO_CHOOSE_METHOD;
							}
						}

						_ui->display("create_alignment", game,
									 static_cast<int>(_stage));

						if (!_controller->has_flag("show_create") &&
							_controller->has_flag("show_method")) {
							return BACK_TO_CHOOSE_METHOD;
						} else if (!_controller->has_flag("show_create") &&
								   _controller->has_flag(
									   "show_training_grounds")) {
							return BACK_TO_TRAINING_GROUNDS;
						}
					}
				}
			}

			// done = true;
		}
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Create::stop([[maybe_unused]] Game *game) -> int {

	_controller->unset_flag("show_create");
	_stage = Enums::Character::Stage::ENTER_NAME;

	return 0;
}