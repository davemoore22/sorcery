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

#include "training/reclass.hpp"
#include "backends/imgui_impl_sdl2.h"		// for SDL_Event
#include "core/audioplayer.hpp"				// for AudioPlayer
#include "core/context.hpp"					// for Context
#include "core/controller/controller.hpp"	// for Controller
#include "core/controller/inputhandler.hpp" // For ControllerInputHandler
#include "core/enum.hpp"					// for Screen
#include "display/ui/ui.hpp"				// for UI
#include "drawables/define.hpp"				// for BACK_TO_EDIT, ABORT_GAME
#include <SDL_events.h>						// for SDL_PollEvent
#include <any>								// for any
#include <string>							// for basic_string

Sorcery::Reclass::Reclass(Context &ctx)
	: Module{ctx} {

	_initialise();
}

Sorcery::Reclass::~Reclass() {}

auto Sorcery::Reclass::_initialise() -> bool {

	return true;
}

auto Sorcery::Reclass::start() -> int {

	_ctx.controller->go_to(Enums::Screen::RECLASS);
	_ctx.controller->unset_flag("want_reclassed_ok");

	show_immediately();

	_ctx.audio->set_volume(1.0f);

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

			if (_ctx.controller->input->back(event))
				return BACK_TO_EDIT;
		}

		_ctx.ui->display_screen(Enums::Screen::RECLASS, _ctx.game);

		_ctx.tick();

		if (_ctx.controller->has_flag("want_reclassed_ok")) {

			_ctx.controller->unset_flag("want_reclassed_ok");

			return BACK_TO_EDIT;
		}

		if (!_ctx.controller->wants(Enums::Screen::RECLASS) &&
			_ctx.controller->wants(Enums::Screen::EDIT))
			return BACK_TO_EDIT;
	}

	return ABORT_GAME;
}
auto Sorcery::Reclass::stop() -> int {

	_ctx.controller->go_to(Enums::Screen::EDIT);

	return 0;
}