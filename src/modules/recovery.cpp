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

#include "modules/recovery.hpp"
#include "backends/imgui_impl_sdl2.h"	 // for SDL_Event
#include "core/context.hpp"				 // for Context
#include "core/controller.hpp"			 // for Controller
#include "core/enum.hpp"				 // for Screen, CharacterSlot
#include "display/ui/ui.hpp"			 // for UI
#include "drawables/define.hpp"			 // for ABORT_GAME, BACK_TO_STAY
#include "types/character/character.hpp" // for Character
#include "types/game.hpp"				 // for Game
#include <SDL_events.h>					 // for SDL_PollEvent
#include <algorithm>					 // for min
#include <any>							 // for any
#include <map>							 // for map

namespace {

struct RecoveryRate {
		int cost;
		int hp;
};

constexpr auto recovery_rate(const int mode) -> RecoveryRate {

	switch (mode) {
	case Sorcery::RECOVERY_MODE_COST_10:
		return {10, 1};

	case Sorcery::RECOVERY_MODE_COST_50:
		return {50, 3};

	case Sorcery::RECOVERY_MODE_COST_200:
		return {200, 7};

	case Sorcery::RECOVERY_MODE_COST_500:
		return {500, 10};

	default:
		return {};
	}
}

} // namespace

Sorcery::Recovery::Recovery(Context &ctx)
	: Module{ctx} {

	_initialise();
}

auto Sorcery::Recovery::_initialise() -> bool {

	_character = nullptr;
	_mode = 0;
	_finished = false;
	_rest_tick = 0;

	return true;
}

// Napping timer only ever runs once.
auto Sorcery::Recovery::_callback_napping(std::uint32_t, void *param)
	-> std::uint32_t {

	auto *recovery{static_cast<Recovery *>(param)};

	recovery->_finished = true;

	return 0;
}

// Recuperation continues while the character needs healing and can afford
// another week.
auto Sorcery::Recovery::_callback_recuperating(std::uint32_t, void *param)
	-> std::uint32_t {

	auto *recovery{static_cast<Recovery *>(param)};
	auto *character{recovery->_character};

	const auto [cost, hp]{recovery_rate(recovery->_mode)};

	if (cost == 0) {
		recovery->_finished = true;
		return 0;
	}

	const auto current_hp{character->get_current_hp()};
	const auto max_hp{character->get_max_hp()};
	const auto current_gold{character->get_gold()};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
	// Cannot buy another week's recuperation.
	if (current_hp >= max_hp || current_gold < cost) {
		recovery->_finished = true;
		return 0;
	}
#pragma GCC diagnostic pop

	// One week passes.
	character->set_current_hp(std::min(current_hp + hp, max_hp));

	character->set_gold(current_gold - cost);
	character->set_age(character->get_age() + 1);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
	// Stop immediately if no further week is required/affordable.
	if (character->get_current_hp() >= max_hp || character->get_gold() < cost) {

		recovery->_finished = true;
		return 0;
	}
#pragma GCC diagnostic pop
	return 1000;
}

auto Sorcery::Recovery::start(const int mode) -> int {

	_ctx.controller->go_to(Enums::Screen::RECOVERY);

	show_immediately();

	_character = &_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STAY));

	_mode = mode;
	_finished = false;

	if (mode == RECOVERY_MODE_FREE)
		_rest_tick = SDL_AddTimer(1000, &Recovery::_callback_napping, this);
	else
		_rest_tick =
			SDL_AddTimer(1000, &Recovery::_callback_recuperating, this);

	while (true) {

		SDL_Event event{};
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
				return BACK_TO_STAY;
		}

		_ctx.ui->display_screen(Enums::Screen::RECOVERY, mode);
		_ctx.tick();

		if (_finished || !_ctx.controller->wants(Enums::Screen::RECOVERY))
			return CHECK_FOR_LEVEL_GAIN;
	}
}

auto Sorcery::Recovery::stop() -> int {

	if (_rest_tick != 0) {
		SDL_RemoveTimer(_rest_tick);
		_rest_tick = 0;
	}

	_ctx.controller->go_to(Enums::Screen::STAY);

	return 0;
}