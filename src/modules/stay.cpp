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
#include "backends/imgui_impl_sdl2.h"	  // for SDL_Event
#include "core/context.hpp"				  // for Context
#include "core/controller/controller.hpp" // for Controller
#include "core/enum.hpp"				  // for Screen, CharacterSlot
#include "display/ui/ui.hpp"			  // for UI
#include "drawables/define.hpp"			  // for ABORT_GAME, BACK_TO_INN
#include "modules/recovery.hpp"			  // for Recovery
#include "modules/result.hpp"			  // for Result, ResultType
#include "types/character/character.hpp"  // for Character
#include "types/character/create.hpp"	  // for CharacterCreate
#include "types/character/magic.hpp"	  // for CharacterMagic
#include "types/game.hpp"				  // for Game
#include <SDL_events.h>					  // for SDL_PollEvent
#include <any>							  // for any
#include <array>						  // for array
#include <map>							  // for map
#include <string>						  // for basic_string

Sorcery::Stay::Stay(Context &ctx)
	: Module{ctx} {

	_recovery = std::make_unique<Recovery>(_ctx);
	_result = std::make_unique<Result>(_ctx);

	_initialise();
};

Sorcery::Stay::~Stay() {}

auto Sorcery::Stay::_initialise() -> bool {

	_ctx.controller->set_selected("room_selected", -1);

	return true;
}

auto Sorcery::Stay::start() -> int {

	// Unlike what happens in the start() methods in other modules, we don't
	// call controller->initialise() here, as this module requires we know what
	// character we have selected to stay at the inn!
	_ctx.controller->go_to(Enums::Screen::STAY);

	show_immediately();

	while (true) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			switch (process_event(
				event,
				{.menu_key = true, .quicksave = false, .quickload = false})) {

			case ModuleEvent::ABORT:
				return ABORT_GAME;

			case ModuleEvent::QUICKLOAD:
				continue;

			case ModuleEvent::NONE:
				break;
			}

			if (_ctx.controller->check_for_back(event))
				return BACK_TO_INN;
		}

		_ctx.ui->display_screen(Enums::Screen::STAY, _ctx.game);
		_ctx.tick();

		const auto room{_ctx.controller->get_selected("room_selected")};

		constexpr std::array recovery_modes{
			RECOVERY_MODE_FREE,		RECOVERY_MODE_COST_10,
			RECOVERY_MODE_COST_50,	RECOVERY_MODE_COST_200,
			RECOVERY_MODE_COST_500,
		};

		// A room has been selected.
		if (room >= 0 && room < static_cast<int>(recovery_modes.size())) {

			auto &character{_ctx.game->characters.at(
				_ctx.controller->get_character(Enums::CharacterSlot::STAY))};

			// Age is stored in weeks; remember the current year so we can
			// detect whether paid recovery crosses a birthday.
			const auto before_age{character.get_age() / 52};

			// Carry out the selected form of recovery.
			const auto mode{recovery_modes[room]};

			const auto recovery_result{_recovery->start(mode)};

			if (recovery_result == ABORT_GAME)
				return ABORT_GAME;

			_recovery->stop();

			// All forms of rest replenish spells, including napping.
			character.magic().replenish_spells();

			// Only paid recovery advances age, so a nap can never cause
			// this condition to become true.
			const auto after_age{character.get_age() / 52};

			const auto recovery_flags{after_age > before_age ? RECOVERY_BIRTHDAY
															 : 0};

			// Work out which result screen is required.
			auto result_type{ResultType::NO_LEVEL_UP};

			if (character.get_cur_xp() >= character.get_next_xp()) {
				character.create().level_up();
				result_type = ResultType::LEVEL_UP;
			}

			// Display the result of the stay.
			const auto result{_result->start(result_type, recovery_flags)};

			if (result == ABORT_GAME)
				return ABORT_GAME;

			_result->stop();

			return BACK_TO_INN;
		}

		// Selecting Return from rest_menu sets room_selected to -1 and
		// changes the requested screen back to the Inn.
		if (!_ctx.controller->wants(Enums::Screen::STAY) &&
			_ctx.controller->wants(Enums::Screen::INN))
			return BACK_TO_INN;
	}
}

auto Sorcery::Stay::stop() -> int {

	_ctx.controller->set_selected("room_selected", -1);
	_ctx.controller->go_to(Enums::Screen::INN);

	return 0;
}