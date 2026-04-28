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

#include "frontend/bestiary.hpp"
#include "common/macro.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"

Sorcery::Bestiary::Bestiary(Context &ctx)
	: _ctx{ctx} {

	_initialise();
};

auto Sorcery::Bestiary::_initialise() -> bool {

	_ctx.controller->set_selected("bestiary_selected", 0); // Bubbly Slime

	return true;
}

auto Sorcery::Bestiary::start() -> int {

	_ctx.controller->go_to(Enums::Screen::BESTIARY);
	_ctx.controller->initialise();

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

		_ctx.ui->display(Enums::Screen::BESTIARY);

		// If we have selected something, let's action it - either return to the
		// calling object, or handle front-end stuff like options, license, or
		// compendium here
		if (_ctx.controller->want_to_abort())
			return ABORT_GAME;
		else if (!_ctx.controller->wants(Enums::Screen::BESTIARY))
			return GO_TO_COMPENDIUM;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Bestiary::stop() -> int {

	_ctx.controller->go_to(Enums::Screen::COMPENDIUM);

	return 0;
}