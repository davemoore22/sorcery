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

#include "training/roster.hpp"
#include "common/macro.hpp"
#include "core/application.hpp"
#include "core/audioplayer.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/define.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "display/display.hpp"
#include "display/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "modules/inspect.hpp"
#include "resources/define.hpp"
#include "training/create.hpp"
#include "types/game.hpp"

Sorcery::Roster::Roster(Context &ctx)
	: Module{ctx} {

	_initialise();

	_inspect = std::make_unique<Inspect>(_ctx);
};

Sorcery::Roster::~Roster() {}

auto Sorcery::Roster::_initialise() -> bool {

	return true;
}

auto Sorcery::Roster::start() -> int {

	_ctx.controller->go_to(Enums::Screen::ROSTER);
	_ctx.controller->initialise();

	// Note that we do not create any inspect character modals here as the
	// inspection is read only

	show_immediately();

	_ctx.audio->set_volume(1.0f);
	_ctx.controller->clear_character(Enums::CharacterSlot::INSPECT);

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event;
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

			if (_ctx.controller->check_for_back(event))
				return BACK_TO_TRAINING_GROUNDS;
		}

		_ctx.ui->display(Enums::Screen::ROSTER, _ctx.game);
		_ctx.tick();

		if (!_ctx.controller->wants(Enums::Screen::ROSTER) &&
			_ctx.controller->wants(Enums::Screen::TRAINING)) {
			_ctx.game->save_game();
			return BACK_TO_TRAINING_GROUNDS;
		} else if (_ctx.controller->has_character(
					   Enums::CharacterSlot::INSPECT)) {
			const auto result{_inspect->start(
				INSPECT_MODE_BASE,
				_ctx.controller->get_character(Enums::CharacterSlot::INSPECT))};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_inspect->stop(INSPECT_MODE_BASE);
			_ctx.controller->go_to(Enums::Screen::ROSTER);
			_ctx.controller->clear_character(Enums::CharacterSlot::INSPECT);
		}
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Roster::stop() -> int {

	return 0;
}