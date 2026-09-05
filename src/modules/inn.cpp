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

#include "modules/inn.hpp"
#include "backends/imgui_impl_sdl2.h"		// for SDL_Event
#include "core/context.hpp"					// for Context
#include "core/controller/controller.hpp"	// for Controller
#include "core/controller/inputhandler.hpp" // For ControllerInputHandler
#include "core/enum.hpp"					// for CharacterSlot, Screen
#include "display/ui/popupmanager.hpp"		// for PopupManager
#include "display/ui/ui.hpp"				// for UI
#include "drawables/define.hpp" // for ABORT_GAME, INSPECT_MODE_ACTIONS
#include "drawables/modal.hpp"	// for Modal
#include "modules/inspect.hpp"	// for Inspect
#include "modules/stay.hpp"		// for Stay
#include <SDL_events.h>			// for SDL_PollEvent
#include <any>					// for any
#include <string>				// for basic_string

Sorcery::Inn::Inn(Context &ctx)
	: Module{ctx} {

	_inspect = std::make_unique<Inspect>(_ctx);
	_stay = std::make_unique<Stay>(_ctx);

	_initialise();
};

Sorcery::Inn::~Inn() {}

auto Sorcery::Inn::_initialise() -> bool {

	_ctx.controller->set_selected("party_panel_selected", 0);

	return true;
}

auto Sorcery::Inn::start() -> int {

	show_immediately();

	_ctx.controller->initialise();
	_ctx.controller->go_to(Enums::Screen::INN);

	_ctx.controller->clear_character(Enums::CharacterSlot::INSPECT);
	_ctx.controller->clear_character(Enums::CharacterSlot::STAY);

	// Main loop
	while (true) {

		SDL_Event event{};
		while (SDL_PollEvent(&event)) {

			switch (process_event(
				event,
				{.menu_key = true, .quicksave = false, .quickload = false})) {

			case ModuleEvent::ABORT:
				return abort();

			case ModuleEvent::QUICKLOAD:
				continue;

			case ModuleEvent::NONE:
				break;
			}

			if (_ctx.controller->input->back(event)) {

				if (_ctx.ui->popup_manager->active())
					_ctx.ui->popup_manager->close();

				return BACK_TO_CASTLE;
			}
		}

		_ctx.ui->display_screen(Enums::Screen::INN, _ctx.game);
		_ctx.tick();

		if (!_ctx.controller->wants(Enums::Screen::INN) &&
			_ctx.controller->wants(Enums::Screen::CASTLE))
			return BACK_TO_CASTLE;

		if (_ctx.controller->has_character(Enums::CharacterSlot::INSPECT)) {
			const auto result{_inspect->start(
				INSPECT_MODE_BASE | INSPECT_MODE_ACTIONS,
				_ctx.controller->get_character(Enums::CharacterSlot::INSPECT))};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_inspect->stop(INSPECT_MODE_BASE | INSPECT_MODE_ACTIONS);
			_ctx.controller->clear_character(Enums::CharacterSlot::INSPECT);
		} else if (_ctx.controller->has_character(Enums::CharacterSlot::STAY)) {
			const auto result{_stay->start()};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_stay->stop();
			_ctx.controller->clear_character(Enums::CharacterSlot::STAY);
		}
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Inn::stop() -> int {

	_ctx.controller->go_to(Enums::Screen::CASTLE);
	_ctx.controller->clear_character(Enums::CharacterSlot::INSPECT);
	_ctx.controller->clear_character(Enums::CharacterSlot::STAY);

	return 0;
}