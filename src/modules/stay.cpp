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

#include "modules/stay.hpp"
#include "common/macro.hpp"
#include "core/application.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/define.hpp"
#include "core/display.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "modules/recovery.hpp"
#include "modules/result.hpp"
#include "resources/define.hpp"
#include "types/game.hpp"

Sorcery::Stay::Stay(Context &ctx)
	: Module{ctx} {

	_recovery = std::make_unique<Recovery>(_ctx);
	_result = std::make_unique<Result>(_ctx);

	_initialise();
};

Sorcery::Stay::~Stay() {}

auto Sorcery::Stay::_initialise() -> bool {

	_ctx.controller->set_selected("stay_selected", -1);

	return true;
}
auto Sorcery::Stay::start() -> int {

	// Unlike what happens in the start() methods in other modules, we don't
	// call controller->initialise() here, as this module requires we know what
	// character we have selected to stay at the inn!
	_ctx.controller->go_to(Enums::Screen::STAY);

	show_immediately();

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
				return BACK_TO_CASTLE;
		}

		_ctx.ui->display(Enums::Screen::STAY, _ctx.game);
		_ctx.tick();

		// Check for Stay Selected (remember +1 to selection)
		if (_ctx.controller->get_selected("stay_selected") > -1) {

			auto &character{_ctx.game->characters.at(
				_ctx.controller->get_character(Enums::CharacterSlot::STAY))};

			const auto before_age{character.get_age() / 52};

			constexpr std::array recovery_modes{
				RECOVERY_MODE_FREE,		RECOVERY_MODE_COST_10,
				RECOVERY_MODE_COST_50,	RECOVERY_MODE_COST_200,
				RECOVERY_MODE_COST_500,
			};

			const auto selection{
				_ctx.controller->get_selected("stay_selected")};

			if (selection >= 1 &&
				selection <= static_cast<int>(recovery_modes.size())) {

				const auto mode{recovery_modes[selection - 1]};
				const auto result{_recovery->start(mode)};
				if (result == ABORT_GAME)
					return ABORT_GAME;
				_recovery->stop();
			}

			// All forms of rest replenish spells, including napping.
			character.replenish_spells();

			// Do we have a birthday?
			const auto after_age{character.get_age() / 52};
			const auto recovery_flags{after_age > before_age ? RECOVERY_BIRTHDAY
															 : 0};

			// And do we have a level up?
			auto result_type{ResultType::NO_LEVEL_UP};
			if (character.get_cur_xp() >= character.get_next_xp()) {
				character.level_up();
				result_type = ResultType::LEVEL_UP;
			}

			// Display the results of the rest appropriately
			const auto result{_result->start(result_type, recovery_flags)};
			if (result == ABORT_GAME)
				return ABORT_GAME;

			_result->stop();

			return BACK_TO_INN;
		}

		if (!_ctx.controller->wants(Enums::Screen::STAY) &&
			_ctx.controller->wants(Enums::Screen::INN))
			return BACK_TO_INN;
	}

	return ABORT_GAME;
}

auto Sorcery::Stay::stop() -> int {

	_ctx.controller->set_selected("stay_selected", -1);
	_ctx.controller->go_to(Enums::Screen::INN);

	return 0;
}