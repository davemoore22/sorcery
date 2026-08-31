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

#include "training/delete.hpp"
#include "backends/imgui_impl_sdl2.h"		// for SDL_Event
#include "core/audioplayer.hpp"				// for AudioPlayer
#include "core/context.hpp"					// for Context
#include "core/controller/controller.hpp"	// for Controller
#include "core/controller/inputhandler.hpp" // For ControllerInputHandler
#include "core/enum.hpp"					// for CharacterSlot, Screen
#include "display/ui/popupstore.hpp"		// for PopupStore
#include "display/ui/ui.hpp"				// for UI
#include "drawables/define.hpp" // for BACK_TO_TRAINING_GROUNDS, ABOR...
#include "drawables/dialog.hpp" // for Dialog
#include "types/game.hpp"		// for Game
#include <SDL_events.h>			// for SDL_PollEvent
#include <any>					// for any
#include <memory>				// for unique_ptr
#include <string>				// for basic_string

Sorcery::Delete::Delete(Context &ctx)
	: Module{ctx} {

	_initialise();
};

Sorcery::Delete::~Delete() {}

auto Sorcery::Delete::_initialise() -> bool {

	return true;
}

auto Sorcery::Delete::start() -> int {

	_ctx.controller->go_to(Enums::Screen::DELETE);
	_ctx.controller->initialise();

	_ctx.controller->clear_character(Enums::CharacterSlot::EDIT);

	_ctx.controller->unset_flag("want_delete_ok");

	_ctx.ui->popups->dialog_delete->show = false;

	show_immediately();

	_ctx.audio->set_volume(1.0f);

	auto done{false};
	auto confirming{false};

	while (!done) {

		SDL_Event event{};

		while (SDL_PollEvent(&event)) {

			switch (process_event(event)) {

			case ModuleEvent::ABORT:
				done = true;
				break;

			case ModuleEvent::QUICKLOAD:
				continue;

			case ModuleEvent::NONE:
				break;
			}

			if (_ctx.controller->input->back(event)) {

				if (confirming) {

					_ctx.ui->popups->dialog_delete->show = false;

					_ctx.controller->unset_flag("want_delete_ok");

					_ctx.controller->clear_character(
						Enums::CharacterSlot::EDIT);

					confirming = false;

				} else {

					return BACK_TO_TRAINING_GROUNDS;
				}
			}
		}

		_ctx.ui->display_screen(Enums::Screen::DELETE, _ctx.game);

		_ctx.tick();

		// Return selected from the menu.
		if (!_ctx.controller->wants(Enums::Screen::DELETE) &&
			_ctx.controller->wants(Enums::Screen::TRAINING))
			return BACK_TO_TRAINING_GROUNDS;

		// A character has just been selected: open confirmation.
		if (!confirming &&
			_ctx.controller->has_character(Enums::CharacterSlot::EDIT)) {

			_ctx.controller->unset_flag("want_delete_ok");

			_ctx.ui->popups->dialog_delete->show = true;

			confirming = true;

			continue;
		}

		if (confirming) {

			// Yes
			if (_ctx.controller->has_flag("want_delete_ok")) {

				_ctx.controller->unset_flag("want_delete_ok");

				_ctx.ui->popups->dialog_delete->show = false;

				_ctx.game->delete_character(
					_ctx.controller->get_character(Enums::CharacterSlot::EDIT));

				_ctx.controller->clear_character(Enums::CharacterSlot::EDIT);

				_ctx.controller->go_to(Enums::Screen::TRAINING);

				return BACK_TO_TRAINING_GROUNDS;
			}

			// No
			if (!_ctx.ui->popups->dialog_delete->show) {

				_ctx.controller->clear_character(Enums::CharacterSlot::EDIT);

				confirming = false;
			}
		}
	}

	return ABORT_GAME;
}

auto Sorcery::Delete::stop() -> int {

	return 0;
}