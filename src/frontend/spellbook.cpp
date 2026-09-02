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

#include "frontend/spellbook.hpp"
#include "backends/imgui_impl_sdl2.h"		// for SDL_Event
#include "common/enum.hpp"					// for SpellID
#include "core/context.hpp"					// for Context
#include "core/controller/controller.hpp"	// for Controller
#include "core/controller/inputhandler.hpp" // For ControllerInputHandler
#include "core/enum.hpp"					// for Screen
#include "display/ui/popupmanager.hpp"		// for PopupManager
#include "display/ui/ui.hpp"				// for UI
#include "drawables/define.hpp"				// for ABORT_GAME, GO_TO_COMPENDIUM
#include <SDL_events.h>						// for SDL_PollEvent
#include <string>							// for basic_string
#include <utility>							// for to_underlying

Sorcery::SpellBook::SpellBook(Context &ctx)
	: Module{ctx} {

	_initialise();
};

auto Sorcery::SpellBook::_initialise() -> bool {

	_ctx.controller->set_selected(
		"spellbook_selected",
		std::to_underlying(Enums::Magic::SpellID::DUMAPIC));

	return true;
}

auto Sorcery::SpellBook::start() -> int {

	_ctx.controller->go_to(Enums::Screen::SPELLBOOK);
	_ctx.controller->initialise();

	fade_in(Enums::Screen::SPELLBOOK, QUICK_FADE);

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

				return GO_TO_COMPENDIUM;
			}
		}

		_ctx.ui->display_screen(Enums::Screen::SPELLBOOK);
		_ctx.tick();

		// If we have selected something, let's action it - either return to the
		// calling object, or handle front-end stuff like options, license, or
		// compendium here
		if (!_ctx.controller->wants(Enums::Screen::SPELLBOOK))
			return GO_TO_FRONT_END;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::SpellBook::stop() -> int {

	_ctx.controller->go_to(Enums::Screen::COMPENDIUM);

	fade_out(Enums::Screen::SPELLBOOK, QUICK_FADE);

	return 0;
}