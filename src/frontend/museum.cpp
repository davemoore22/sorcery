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

#include "frontend/museum.hpp"
#include "common/macro.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"

Sorcery::Museum::Museum(Context &ctx)
	: _ctx{ctx} {

	_initialise();
};

auto Sorcery::Museum::_initialise() -> bool {

	_ctx.controller->set_selected("museum_selected", 1); // Long Sword

	return true;
}

auto Sorcery::Museum::start() -> int {

	_ctx.controller->initialise("museum");
	_ctx.controller->set_flag("show_museum");

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			// Check for Quit or Back Events
			ImGui_ImplSDL2_ProcessEvent(&event);
			_ctx.controller->check_for_resize(event, _ctx.ui);
			done = _ctx.controller->check_for_abort(event);
			if (_ctx.controller->check_for_back(event))
				return GO_TO_COMPENDIUM;
		}

		_ctx.ui->display("museum");

		// If we have selected something, let's action it - either return to the
		// calling object, or handle front-end stuff like options, license, or
		// compendium here
		if (_ctx.controller->has_flag("want_abort"))
			return ABORT_GAME;
		else if (!_ctx.controller->has_flag("show_museum"))
			return GO_TO_COMPENDIUM;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Museum::stop() -> int {

	_ctx.controller->move_screen("show_museum", "show_compendium");

	return 0;
}