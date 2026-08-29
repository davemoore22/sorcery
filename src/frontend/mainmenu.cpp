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

#include "frontend/mainmenu.hpp"
#include "backends/imgui_impl_sdl2.h" // for SDL_Event
#include "core/audioplayer.hpp"		  // for AudioPlayer
#include "core/context.hpp"			  // for Context
#include "core/controller.hpp"		  // for Controller
#include "core/enum.hpp"			  // for Screen
#include "display/animation.hpp"	  // for Animation
#include "display/ui/popupstore.hpp"  // for PopupStore
#include "display/ui/ui.hpp"		  // for UI
#include "drawables/define.hpp"		  // for ABORT_GAME, MAIN_MENU_CONTINUE...
#include "drawables/dialog.hpp"		  // for Dialog
#include "frontend/compendium.hpp"	  // for Compendium
#include "frontend/license.hpp"		  // for License
#include "frontend/options.hpp"		  // for Options
#include <SDL_events.h>				  // for SDL_PollEvent
#include <chrono>					  // for chrono_literals
#include <string>					  // for basic_string

Sorcery::MainMenu::MainMenu(Context &ctx)
	: Module{ctx} {

	_options = std::make_unique<Options>(_ctx);
	_license = std::make_unique<License>(_ctx);
	_compendium = std::make_unique<Compendium>(_ctx);

	_initialise();
};

Sorcery::MainMenu::~MainMenu() = default;

auto Sorcery::MainMenu::_initialise() -> bool {

	return true;
}

auto Sorcery::MainMenu::start() -> int {

	using namespace std::chrono_literals;

	// Clear all controller and flow flags
	_ctx.controller->initialise();

	// Start relevant animation worker threads
	_ctx.animation->refresh_attract();
	_ctx.animation->start_attract_th();

	fade_in(Enums::Screen::MAINMENU, QUICK_FADE);

	_ctx.audio->set_volume(1.0f);

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event{};
		while (SDL_PollEvent(&event)) {

			switch (process_event(
				event,
				{.menu_key = true, .quicksave = false, .quickload = false})) {

			case ModuleEvent::ABORT:
				done = true;
				break;

			case ModuleEvent::QUICKLOAD:
				continue;

			case ModuleEvent::NONE:
				break;
			}

			_ctx.controller->check_for_back(event,
											_ctx.ui->popups->dialog_exit->show);
		}

		_ctx.tick();

		if (!done) {

			_ctx.ui->display_screen(Enums::Screen::MAINMENU);

			// Check for the results of a Popup Dialog
			if (_ctx.controller->has_flag("want_exit_game"))
				return MAIN_MENU_EXIT_GAME;
			else if (_ctx.controller->has_flag("want_new_game"))
				return MAIN_MENU_NEW_GAME;
			else if (_ctx.controller->want_to_abort())
				return ABORT_GAME;
			else if (_ctx.controller->has_flag("want_continue_game"))
				return MAIN_MENU_CONTINUE_GAME;

			// Check for the results of something being selected from a menu
			if (_ctx.controller->wants(Enums::Screen::COMPENDIUM)) {
				const auto result{_compendium->start()};
				_compendium->stop();
				if (result == ABORT_GAME)
					return ABORT_GAME;
				fade_in(Enums::Screen::MAINMENU, QUICK_FADE);
			} else if (_ctx.controller->wants(Enums::Screen::OPTIONS)) {
				const auto result{_options->start(false)};
				_options->stop();
				if (result == ABORT_GAME)
					return ABORT_GAME;
				fade_in(Enums::Screen::MAINMENU, QUICK_FADE);
			} else if (_ctx.controller->wants(Enums::Screen::LICENSE)) {
				const auto result{_license->start()};
				_license->stop();
				if (result == ABORT_GAME)
					return ABORT_GAME;
				fade_in(Enums::Screen::MAINMENU, QUICK_FADE);
			}
		}
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::MainMenu::stop() -> int {

	fade_out(Enums::Screen::MAINMENU, QUICK_FADE);

	return 0;
}