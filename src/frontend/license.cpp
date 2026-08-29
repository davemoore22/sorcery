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

#include "frontend/license.hpp"
#include "backends/imgui_impl_sdl2.h" // for SDL_Event
#include "core/context.hpp"			  // for Context
#include "core/controller.hpp"		  // for Controller
#include "core/enum.hpp"			  // for Screen
#include "display/ui/ui.hpp"		  // for UI
#include "drawables/define.hpp"		  // for GO_TO_FRONT_END, ABORT_GAME
#include "resources/define.hpp"		  // for LICENSE_FILE
#include <SDL_events.h>				  // for SDL_PollEvent
#include <any>						  // for any
#include <filesystem>				  // for path
#include <fstream>					  // for basic_ifstream, ifstream
#include <iterator>					  // for istreambuf_iterator, operator==
#include <string>					  // for basic_string

Sorcery::License::License(Context &ctx)
	: Module{ctx} {

	_initialise();
};

auto Sorcery::License::_initialise() -> bool {

	auto file_path{_ctx.get_file(LICENSE_FILE).string()};

	if (std::ifstream file{(file_path.c_str()), std::ifstream::in};
		file.good()) {

		_license_text.assign((std::istreambuf_iterator<char>(file)),
							 (std::istreambuf_iterator<char>()));
	}

	return true;
}

auto Sorcery::License::start() -> int {

	_ctx.controller->go_to(Enums::Screen::LICENSE);
	_ctx.controller->initialise();

	fade_in(Enums::Screen::LICENSE, QUICK_FADE);

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event;
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

			if (_ctx.controller->check_for_back(event))
				return GO_TO_FRONT_END;
		}

		_ctx.ui->display_screen(Enums::Screen::LICENSE, _license_text);

		_ctx.tick();

		if (!_ctx.controller->wants(Enums::Screen::LICENSE))
			return GO_TO_FRONT_END;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::License::stop() -> int {

	_ctx.controller->go_to(Enums::Screen::MAINMENU);

	fade_out(Enums::Screen::LICENSE, QUICK_FADE);

	return 0;
}