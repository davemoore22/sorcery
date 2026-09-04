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
#include "backends/imgui_impl_sdl2.h"		// for SDL_Event
#include "core/audioplayer.hpp"				// for AudioPlayer
#include "core/context.hpp"					// for Context
#include "core/controller/controller.hpp"	// for Controller
#include "core/controller/inputhandler.hpp" // For ControllerInputHandler
#include "core/enum.hpp"					// for Screen, CharacterSlot
#include "display/ui/popupmanager.hpp"		// for PopupManager
#include "display/ui/popupstore.hpp"		// for PopupStore
#include "display/ui/ui.hpp"				// for UI
#include "drawables/define.hpp"	 // for ABORT_GAME, INSPECT_MODE_ACTIONS
#include "drawables/dialog.hpp"	 // for Dialog
#include "drawables/modal.hpp"	 // for Modal
#include "modules/inspect.hpp"	 // for Inspect
#include "modules/restart.hpp"	 // for Restart
#include "training/training.hpp" // for Training
#include "types/game.hpp"		 // for Game
#include <SDL_events.h>			 // for SDL_PollEvent
#include <any>					 // for any
#include <string>				 // for basic_string

Sorcery::EdgeOfTown::EdgeOfTown(Context &ctx)
	: Module{ctx} {

	_initialise();

	_training_grounds = std::make_unique<Training>(_ctx);
	_restart = std::make_unique<Restart>(_ctx);
	_inspect = std::make_unique<Inspect>(_ctx);
};

Sorcery::EdgeOfTown::~EdgeOfTown() {}

auto Sorcery::EdgeOfTown::_initialise() -> bool {

	_ctx.controller->set_selected("party_panel_selected", 0);

	return true;
}

auto Sorcery::EdgeOfTown::start(const int mode) -> int {

	_ctx.controller->go_to(Enums::Screen::EDGEOFTOWN);
	_ctx.controller->initialise();

	fade_in(Enums::Screen::EDGEOFTOWN, QUICK_FADE);

	_ctx.ui->popups->create_dynamic_modal("modal_trade");
	_ctx.ui->popups->create_dynamic_modal("modal_give");

	_ctx.ui->popups->modal_give->show = false;
	_ctx.ui->popups->modal_trade->show = false;

	_ctx.controller->clear_character(Enums::CharacterSlot::INSPECT);

	_ctx.audio->set_volume(1.0f);

	// Main loop
	while (true) {

		SDL_Event event{};
		while (SDL_PollEvent(&event)) {

			switch (process_event(event, {.menu_key = true, .debug = true})) {

			case ModuleEvent::ABORT:
				return abort();

			case ModuleEvent::QUICKLOAD:
				continue;

			case ModuleEvent::NONE:
				break;
			}

			if (_ctx.controller->input->back(event)) {

				if (_ctx.ui->popup_manager->active()) {

					_ctx.ui->popup_manager->close();

				} else {

					_ctx.ui->popup_manager->open_dialog(
						"main_menu:dialog_leave",
						Enums::Layout::DialogType::CONFIRM);
				}

				continue;
			}
		}

		_ctx.ui->display_screen(Enums::Screen::EDGEOFTOWN, _ctx.game);
		_ctx.tick();

		if (_ctx.ui->popup_manager->consume_accepted("dialog_leave")) {

			_ctx.game->move_party_to_tavern();
			_ctx.game->save_game();
			_ctx.controller->set_game(nullptr);

			return LEAVE_GAME;

		} else if (!_ctx.controller->wants(Enums::Screen::EDGEOFTOWN) &&
				   _ctx.controller->wants(Enums::Screen::CASTLE)) {

			return EDGE_OF_TOWN_GO_TO_CASTLE;
		}

		// Check for the results of something being selected from a menu
		if (_ctx.controller->wants(Enums::Screen::TRAINING)) {
			_ctx.game->move_party_to_tavern();
			_ctx.game->save_game();
			const auto result{_training_grounds->start()};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_training_grounds->stop();

			// Needs a fade in here
			fade_in(Enums::Screen::EDGEOFTOWN, QUICK_FADE);
		} else if (_ctx.controller->wants(Enums::Screen::RESTART)) {
			const auto result{_restart->start()};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_restart->stop();
			if (_ctx.controller->has_flag("want_restart_expedition"))
				return RESTART_MAZE;
		} else if (_ctx.controller->wants(Enums::Screen::ENGINE))
			return EDGE_OF_TOWN_GO_TO_MAZE;
		else if (_ctx.controller->has_character(
					 Enums::CharacterSlot::INSPECT)) {
			const auto result{_inspect->start(
				INSPECT_MODE_BASE | INSPECT_MODE_ACTIONS,
				_ctx.controller->get_character(Enums::CharacterSlot::INSPECT))};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_inspect->stop(INSPECT_MODE_BASE | INSPECT_MODE_ACTIONS);
			_ctx.controller->clear_character(Enums::CharacterSlot::INSPECT);
		}
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::EdgeOfTown::stop() -> int {

	fade_out(Enums::Screen::EDGEOFTOWN, QUICK_FADE);

	return 0;
}