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
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "gui/input.hpp"
#include "types/character.hpp"
#include "types/game.hpp"

Sorcery::Create::Create(Context &ctx)
	: _ctx{ctx} {

	_initialise();
};

Sorcery::Create::~Create() {};

auto Sorcery::Create::_initialise() -> bool {

	_stage = Enums::Character::Stage::ENTER_NAME;

	return true;
}

auto Sorcery::Create::start() -> int {

	_ctx.controller->initialise("create");
	_ctx.controller->set_flag("show_create");
	_ctx.controller->set_flag("want_enter_name");
	_ctx.controller->unset_flag("want_choose_race");
	_ctx.controller->unset_flag("want_choose_alignment");
	_ctx.controller->unset_flag("want_choose_class");
	_ctx.controller->unset_flag("want_choose_confirm");
	_ctx.controller->set_method(Enums::Character::Method::NO_METHOD);
	_ctx.controller->set_selected("class_selected", 8);

	std::shared_ptr<Character> candidate = std::make_shared<Character>(&_ctx);

	_ctx.controller->inject_character(candidate);
	candidate->reset(Enums::Character::Stage::ENTER_NAME);

	_ctx.ui->first_frame = true;

	// Main loop
	auto done{false};
	SDL_Event event;
	while (!done) {

		while (SDL_PollEvent(&event)) {

			// Check for Quit Events
			ImGui_ImplSDL2_ProcessEvent(&event);
			done = _ctx.controller->check_for_abort(event);

			// Check for Window Resize
			_ctx.controller->check_for_resize(event, _ctx.ui);

			// Check for Back Event
			if (_ctx.controller->check_for_back(event)) {
				return BACK_TO_CHOOSE_METHOD;
			}
		}

		_ctx.ui->display("create_name", _ctx.game, static_cast<int>(_stage));

		if (!_ctx.controller->has_flag("show_create") &&
			_ctx.controller->has_flag("show_method")) {
			return BACK_TO_CHOOSE_METHOD;
		} else if (!_ctx.controller->has_flag("show_create") &&
				   _ctx.controller->has_flag("show_training")) {
			return BACK_TO_TRAINING_GROUNDS;
		}

		// Check to see if we have finished entering the name (the stage and
		// name are set in controller->handle_input_button_click())
		if (!_ctx.controller->has_flag("want_enter_name") &&
			candidate->get_stage() != Enums::Character::Stage::ENTER_NAME) {

			while (!done) {

				// Move to the next stage
				while (SDL_PollEvent(&event)) {

					// Check for Quit Events
					ImGui_ImplSDL2_ProcessEvent(&event);
					done = _ctx.controller->check_for_abort(event);

					// Check for Window Resize
					_ctx.controller->check_for_resize(event, _ctx.ui);

					// Check for Back Event
					if (_ctx.controller->check_for_back(event)) {
						return BACK_TO_CHOOSE_METHOD;
					}
				}

				_ctx.ui->display("create_race", _ctx.game,
								 static_cast<int>(_stage));

				if (!_ctx.controller->has_flag("show_create") &&
					_ctx.controller->has_flag("show_method")) {
					return BACK_TO_CHOOSE_METHOD;
				} else if (!_ctx.controller->has_flag("show_create") &&
						   _ctx.controller->has_flag("show_training")) {
					return BACK_TO_TRAINING_GROUNDS;
				}

				if (!_ctx.controller->has_flag("want_choose_race") &&
					_ctx.controller->has_flag("want_choose_alignment")) {

					while (!done) {

						// Move to the next stage
						while (SDL_PollEvent(&event)) {

							// Check for Quit Events
							ImGui_ImplSDL2_ProcessEvent(&event);
							done = _ctx.controller->check_for_abort(event);

							// Check for Window Resize
							_ctx.controller->check_for_resize(event, _ctx.ui);

							// Check for Back Event
							if (_ctx.controller->check_for_back(event)) {
								return BACK_TO_CHOOSE_METHOD;
							}
						}

						_ctx.ui->display("create_alignment", _ctx.game,
										 static_cast<int>(_stage));

						if (!_ctx.controller->has_flag("show_create") &&
							_ctx.controller->has_flag("show_method")) {
							return BACK_TO_CHOOSE_METHOD;
						} else if (!_ctx.controller->has_flag("show_create") &&
								   _ctx.controller->has_flag("show_training")) {
							return BACK_TO_TRAINING_GROUNDS;
						}

						if (!_ctx.controller->has_flag(
								"want_choose_alignment") &&
							_ctx.controller->has_flag("want_choose_class")) {

							while (!done) {

								// Move to the next stage
								while (SDL_PollEvent(&event)) {

									// Check for Quit Events
									ImGui_ImplSDL2_ProcessEvent(&event);
									done =
										_ctx.controller->check_for_abort(event);

									// Check for Window Resize
									_ctx.controller->check_for_resize(event,
																	  _ctx.ui);

									// Check for Back Event
									if (_ctx.controller->check_for_back(
											event)) {
										return BACK_TO_CHOOSE_METHOD;
									}
								}

								_ctx.ui->display("create_class", _ctx.game,
												 static_cast<int>(_stage));

								if (!_ctx.controller->has_flag("show_create") &&
									_ctx.controller->has_flag("show_method")) {
									return BACK_TO_CHOOSE_METHOD;
								} else if (!_ctx.controller->has_flag(
											   "show_create") &&
										   _ctx.controller->has_flag(
											   "show_training")) {
									return BACK_TO_TRAINING_GROUNDS;
								}

								if (!_ctx.controller->has_flag(
										"want_choose_class") &&
									_ctx.controller->has_flag(
										"want_choose_confirm")) {

									while (!done) {

										// Move to the next stage
										while (SDL_PollEvent(&event)) {

											// Check for Quit Events
											ImGui_ImplSDL2_ProcessEvent(&event);
											done = _ctx.controller
													   ->check_for_abort(event);

											// Check for Window Resize
											_ctx.controller->check_for_resize(
												event, _ctx.ui);

											// Check for Back Event
											if (_ctx.controller->check_for_back(
													event)) {
												return BACK_TO_CHOOSE_METHOD;
											}
										}

										if (!_ctx.controller->has_flag(
												"show_create") &&
											_ctx.controller->has_flag(
												"show_method")) {
											return BACK_TO_CHOOSE_METHOD;
										} else if (!_ctx.controller->has_flag(
													   "show_create") &&
												   _ctx.controller->has_flag(
													   "show_training")) {
											return BACK_TO_TRAINING_GROUNDS;
										}

										_ctx.ui->display(
											"create_confirm", _ctx.game,
											static_cast<int>(_stage));
									}
								}
							}
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

auto Sorcery::Create::stop() -> int {

	_ctx.controller->unset_flag("show_create");
	_stage = Enums::Character::Stage::ENTER_NAME;

	return 0;
}