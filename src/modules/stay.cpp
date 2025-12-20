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

#include "modules/stay.hpp"
#include "common/macro.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "modules/levelup.hpp"
#include "modules/nolevelup.hpp"
#include "modules/recovery.hpp"
#include "types/game.hpp"

Sorcery::Stay::Stay(Context &ctx)
	: _ctx{ctx} {

	_recovery = std::make_unique<Recovery>(_ctx);
	_no_level_up = std::make_unique<NoLevelUp>(_ctx);
	_level_up = std::make_unique<LevelUp>(_ctx);

	_initialise();
};

Sorcery::Stay::~Stay() {}

auto Sorcery::Stay::_initialise() -> bool {

	_ctx.controller->set_selected("stay_selected", 0);

	return true;
}

auto Sorcery::Stay::start() -> int {

	// Unlike what happens in the start() methods in other modules, we don't
	// call controller->initialise() here, as this module requires we know what
	// character we have selected to stay at the inn!
	_ctx.controller->go_to(Enums::Screen::STAY);

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
			if (_ctx.controller->check_for_back(event))
				return BACK_TO_CASTLE;

			// Check for Stay Selected (remember +1 to selection)
			if (_ctx.controller->get_selected("stay_selected") > 0) {

				// Get Age beforehand
				auto &character{_ctx.game->characters.at(
					_ctx.controller->get_character("stay"))};
				const auto before_age{character.get_age() % 52};

				// Remember in this case its +1 the usual offset
				switch (_ctx.controller->get_selected("stay_selected")) {
				case 1:
					_recovery->start(RECOVERY_MODE_FREE);
					_recovery->stop();
					break;
				case 2:
					_recovery->start(RECOVERY_MODE_COST_10);
					_recovery->stop();
					break;
				case 3:
					_recovery->start(RECOVERY_MODE_COST_50);
					_recovery->stop();
					break;
				case 4:
					_recovery->start(RECOVERY_MODE_COST_200);
					_recovery->stop();
					break;
				case 5:
					_recovery->start(RECOVERY_MODE_COST_500);
					_recovery->stop();
					break;
				default:
					break;
				}

				// Now check for Level up!
				const auto after_age{character.get_age() % 52};
				const auto current_xp{character.get_cur_xp()};
				const auto next_xp{character.get_next_xp()};
				if (next_xp - current_xp > 0) {

					// No Level Up!
					_no_level_up->start(
						after_age > before_age ? RECOVERY_BIRTHDAY : 0);
					_no_level_up->stop();
				} else {

					// Level Up!
					character.level_up();
					_level_up->start(after_age > before_age ? RECOVERY_BIRTHDAY
															: 0);
					_level_up->stop();
				}

				return BACK_TO_INN;
			}
		}

		_ctx.ui->display(Enums::Screen::STAY, _ctx.game);

		if (!_ctx.controller->wants(Enums::Screen::STAY) &&
			_ctx.controller->wants(Enums::Screen::INN))
			return BACK_TO_INN;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Stay::stop() -> int {

	_ctx.controller->set_selected("stay_selected", 0);
	_ctx.controller->go_to(Enums::Screen::INN);

	return 0;
}