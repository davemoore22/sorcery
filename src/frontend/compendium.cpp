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

#include "frontend/compendium.hpp"
#include "backends/imgui_impl_sdl2.h"		// for SDL_Event
#include "core/context.hpp"					// for Context
#include "core/controller/controller.hpp"	// for Controller
#include "core/controller/inputhandler.hpp" // For ControllerInputHandler
#include "core/enum.hpp"					// for Screen
#include "display/ui/popupmanager.hpp"		// for PopupManager
#include "display/ui/ui.hpp"				// for UI
#include "drawables/define.hpp"				// for ABORT_GAME, GO_TO_FRONT_END
#include "frontend/atlas.hpp"				// for Atlas
#include "frontend/bestiary.hpp"			// for Bestiary
#include "frontend/license.hpp"				// for License
#include "frontend/museum.hpp"				// for Museum
#include "frontend/spellbook.hpp"			// for SpellBook
#include <SDL_events.h>						// for SDL_PollEvent
#include <string>							// for basic_string

Sorcery::Compendium::Compendium(Context &ctx)
	: Module{ctx} {

	_bestiary = std::make_unique<Bestiary>(_ctx);
	_museum = std::make_unique<Museum>(_ctx);
	_atlas = std::make_unique<Atlas>(_ctx);
	_spellbook = std::make_unique<SpellBook>(_ctx);

	_initialise();
};

Sorcery::Compendium::~Compendium() {}

auto Sorcery::Compendium::_initialise() -> bool {

	_ctx.controller->set_selected("compendium_selected", 0);

	return true;
}

auto Sorcery::Compendium::start() -> int {

	_ctx.controller->go_to(Enums::Screen::COMPENDIUM);
	_ctx.controller->initialise();

	fade_in(Enums::Screen::COMPENDIUM, QUICK_FADE);

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

				return GO_TO_FRONT_END;
			}
		}

		_ctx.ui->display_screen(Enums::Screen::COMPENDIUM);

		_ctx.tick();

		// If we have selected something, let's action it - either return to the
		// calling object, or handle front-end stuff like options, license, or
		// compendium here
		if (_ctx.controller->wants(Enums::Screen::BESTIARY)) {
			const auto result{_bestiary->start()};
			_bestiary->stop();
			if (result == ABORT_GAME)
				return ABORT_GAME;
			fade_in(Enums::Screen::COMPENDIUM, QUICK_FADE);
		} else if (_ctx.controller->wants(Enums::Screen::MUSEUM)) {
			const auto result{_museum->start()};
			_museum->stop();
			if (result == ABORT_GAME)
				return ABORT_GAME;
			fade_in(Enums::Screen::COMPENDIUM, QUICK_FADE);
		} else if (_ctx.controller->wants(Enums::Screen::ATLAS)) {
			const auto result{_atlas->start()};
			_atlas->stop();
			if (result == ABORT_GAME)
				return ABORT_GAME;
			fade_in(Enums::Screen::COMPENDIUM, QUICK_FADE);
		} else if (_ctx.controller->wants(Enums::Screen::SPELLBOOK)) {
			const auto result{_spellbook->start()};
			_spellbook->stop();
			if (result == ABORT_GAME)
				return ABORT_GAME;
			fade_in(Enums::Screen::COMPENDIUM, QUICK_FADE);
		}

		if (!_ctx.controller->wants(Enums::Screen::COMPENDIUM))
			return GO_TO_FRONT_END;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Compendium::stop() -> int {

	_ctx.controller->go_to(Enums::Screen::MAINMENU);

	fade_out(Enums::Screen::COMPENDIUM, QUICK_FADE);

	return 0;
}