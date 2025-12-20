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

#include "modules/method.hpp"
#include "common/macro.hpp"
#include "core/application.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "gui/input.hpp"
#include "modules/create.hpp"
#include "types/game.hpp"

Sorcery::Method::Method(Context &ctx)
	: _ctx{ctx} {

	_create = std::make_unique<Create>(_ctx);

	_initialise();
};

Sorcery::Method::~Method() {};

auto Sorcery::Method::_initialise() -> bool {

	//_stage = Enums::Character::Stage::ENTER_NAME;

	return true;
}

auto Sorcery::Method::start() -> int {

	_ctx.controller->go_to(Enums::Screen::METHOD);
	_ctx.controller->initialise(Enums::Screen::METHOD);

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			// Check for Quit Events
			ImGui_ImplSDL2_ProcessEvent(&event);
			done = _ctx.controller->check_for_abort(event);

			// Check for Window Resize
			_ctx.controller->check_for_resize(event, _ctx.ui);

			// Check for Back Event
			if (_ctx.controller->check_for_back(event)) {
				return BACK_TO_TRAINING_GROUNDS;
			}
		}

		_ctx.ui->display(Enums::Screen::METHOD, _ctx.game);

		if (!_ctx.controller->wants(Enums::Screen::METHOD) &&
			_ctx.controller->wants(Enums::Screen::TRAINING)) {
			_ctx.game->save_game();
			return BACK_TO_TRAINING_GROUNDS;
		} else if (_ctx.controller->wants(Enums::Screen::CREATE)) {
			_create->start();
			_create->stop();
			return BACK_TO_TRAINING_GROUNDS;
		}
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Method::stop() -> int {

	return 0;
}