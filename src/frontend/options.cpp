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

#include "frontend/options.hpp"
#include "common/macro.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"

Sorcery::Options::Options(Context &ctx)
	: _ctx{ctx} {

	_initialise();
};

auto Sorcery::Options::_initialise() -> bool {

	_is_in_game = false;

	return true;
}

auto Sorcery::Options::start(const bool is_in_game) -> int {

	_is_in_game = is_in_game;
	_ctx.controller->go_to(Enums::Screen::OPTIONS);
	_ctx.controller->initialise();

	// Main loop
	auto done{false};
	_fullscreen_before = _ctx.get_config(Enums::Config::FULLSCREEN);
	_monochrome_before = _ctx.get_config(Enums::Config::COLOURED_WIREFRAME);
	while (!done) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			// Check for Quit or Back Events
			ImGui_ImplSDL2_ProcessEvent(&event);
			_ctx.controller->check_for_resize(event, _ctx.ui);
			done = _ctx.controller->check_for_abort(event);
			if (_ctx.controller->check_for_back(event))
				return GO_TO_FRONT_END; // Or back to the Game
		}

		_ctx.ui->display(Enums::Screen::OPTIONS);
		if (_ctx.controller->want_to_abort())
			return ABORT_GAME;
		else if (!_ctx.controller->wants(Enums::Screen::OPTIONS))
			return GO_TO_FRONT_END;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Options::stop() -> int {

	auto fullscreen_after{_ctx.get_config(Enums::Config::FULLSCREEN)};
	if (_fullscreen_before != fullscreen_after)
		_ctx.ui->set_fullscreen(fullscreen_after);

	auto monochrome_after{_ctx.get_config(Enums::Config::COLOURED_WIREFRAME)};
	if (_monochrome_before != monochrome_after) {
		_ctx.controller->set_monochrome(monochrome_after);
		_ctx.ui->set_monochrome(monochrome_after);
	}

	if (_is_in_game)
		_ctx.controller->go_to(Enums::Screen::ENGINE);
	else
		_ctx.controller->go_to(Enums::Screen::MAINMENU);

	return 0;
}