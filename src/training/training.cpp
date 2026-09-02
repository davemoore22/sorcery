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

#include "training/training.hpp"
#include "backends/imgui_impl_sdl2.h"		// for SDL_Event
#include "core/audioplayer.hpp"				// for AudioPlayer
#include "core/context.hpp"					// for Context
#include "core/controller/controller.hpp"	// for Controller
#include "core/controller/inputhandler.hpp" // For ControllerInputHandler
#include "core/enum.hpp"					// for Screen
#include "display/ui/popupmanager.hpp"		// for PopupManager
#include "display/ui/ui.hpp"				// for UI
#include "drawables/define.hpp" // for ABORT_GAME, BACK_TO_EDGE_OF_TOWN
#include "training/create.hpp"	// for Create
#include "training/delete.hpp"	// for Delete
#include "training/edit.hpp"	// for Edit
#include "training/roster.hpp"	// for Roster
#include "types/game.hpp"		// for Game
#include <SDL_events.h>			// for SDL_PollEvent
#include <any>					// for any

Sorcery::Training::Training(Context &ctx)
	: Module{ctx} {

	_initialise();

	_create = std::make_unique<Create>(_ctx);
	_roster = std::make_unique<Roster>(_ctx);
	_edit = std::make_unique<Edit>(_ctx);
	_delete = std::make_unique<Delete>(_ctx);
};

Sorcery::Training::~Training() {}

auto Sorcery::Training::_initialise() -> bool {

	return true;
}

auto Sorcery::Training::start() -> int {

	_ctx.controller->go_to(Enums::Screen::TRAINING);
	_ctx.controller->initialise();

	fade_in(Enums::Screen::TRAINING, QUICK_FADE);

	_ctx.audio->set_volume(1.0f);

	// Main loop
	auto done{false};
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

				if (_ctx.ui->popup_manager->active())
					_ctx.ui->popup_manager->close();

				return BACK_TO_EDGE_OF_TOWN;
			}
		}

		_ctx.ui->display_screen(Enums::Screen::TRAINING, _ctx.game);
		_ctx.tick();

		if (!_ctx.controller->wants(Enums::Screen::TRAINING) &&
			_ctx.controller->wants(Enums::Screen::EDGEOFTOWN)) {

			_ctx.game->save_game();

			return BACK_TO_EDGE_OF_TOWN;
		}

		if (_ctx.controller->wants(Enums::Screen::CREATE)) {
			const auto result{_create->start()};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_create->stop();
		} else if (_ctx.controller->wants(Enums::Screen::ROSTER)) {
			const auto result{_roster->start()};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_roster->stop();
		} else if (_ctx.controller->wants(Enums::Screen::EDIT)) {
			const auto result{_edit->start()};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_edit->stop();
		} else if (_ctx.controller->wants(Enums::Screen::DELETE)) {
			const auto result{_delete->start()};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_delete->stop();
		}
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Training::stop() -> int {

	fade_out(Enums::Screen::TRAINING, QUICK_FADE);

	return 0;
}