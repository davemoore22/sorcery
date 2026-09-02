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

#include "modules/heal.hpp"
#include "backends/imgui_impl_sdl2.h"		// for SDL_Event
#include "common/enum.hpp"					// for Status, Random, Random::D100
#include "core/context.hpp"					// for Context
#include "core/controller/controller.hpp"	// for Controller
#include "core/controller/inputhandler.hpp" // For ControllerInputHandler
#include "core/enum.hpp"					// for Screen, CharacterSlot
#include "display/ui/popupmanager.hpp"		// for PopupManager
#include "display/ui/ui.hpp"				// for UI
#include "drawables/define.hpp"				// for BACK_TO_TEMPLE, ABORT_GAME
#include "types/character/character.hpp"	// for Character
#include "types/game.hpp"					// for Game
#include <SDL_events.h>						// for SDL_PollEvent
#include <any>								// for any
#include <format>							// for format
#include <map>								// for map
#include <string>							// for basic_string

Sorcery::Heal::Heal(Context &ctx)
	: Module{ctx} {}

auto Sorcery::Heal::_callback_heal_tick(std::uint32_t, void *param)
	-> std::uint32_t {

	auto *heal{static_cast<Heal *>(param)};

	const auto stage{--heal->_stage};

	return stage == 0 ? 0 : 2000;
}

auto Sorcery::Heal::start() -> int {

	_stage = 4;
	_healing_done = false;
	_heal_tick = 0;

	_ctx.controller->unset_flag("heal_finished");
	_ctx.controller->unset_text("heal_results");
	_ctx.controller->go_to(Enums::Screen::HEAL);

	show_immediately();

	_heal_tick = SDL_AddTimer(2000, &Heal::_callback_heal_tick, this);

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

			if (_ctx.controller->input->back(event)) {

				if (_ctx.ui->popup_manager->active())
					_ctx.ui->popup_manager->close();

				return BACK_TO_TEMPLE;
			}
		}

		const auto stage{_stage.load()};

		if (stage <= 0 && !_healing_done) {

			_try_heal(
				_ctx.controller->get_character(Enums::CharacterSlot::HELP),
				_ctx.controller->get_character(Enums::CharacterSlot::PAY));

			_healing_done = true;

			_ctx.controller->set_flag("heal_finished");
		}

		_ctx.ui->display_screen(Enums::Screen::HEAL, stage);

		_ctx.tick();

		if (!_ctx.controller->wants(Enums::Screen::HEAL))
			return BACK_TO_TEMPLE;
	}
}

auto Sorcery::Heal::_try_heal(int heal_char_id, int pay_char_id) -> bool {

	auto &heal_char{_ctx.game->characters.at(heal_char_id)};

	auto &pay_char{_ctx.game->characters.at(pay_char_id)};

	using enum Enums::Character::Attribute;
	using enum Enums::Character::Location;
	using enum Enums::Character::Status;
	using enum Enums::System::Random;

	const auto cost{heal_char.get_cure_cost()};

	pay_char.grant_gold(-cost);

	const auto old_status{heal_char.get_status()};
	const auto vitality{heal_char.get_cur_attr(VITALITY)};

	auto success = [&](const int hp) {
		heal_char.set_status(OK);
		heal_char.set_current_hp(hp);
		heal_char.set_location(TAVERN);

		heal_char.set_age(heal_char.get_age() + _ctx.get_random(D52));

		_ctx.controller->set_text(
			"heal_results",
			std::format("{} {} {}", _ctx.get_string("TEMPLE_HEALED_PREFIX"),
						heal_char.get_name(),
						_ctx.get_string("TEMPLE_HEALED_SUFFIX")));

		return true;
	};

	if (old_status == HELD || old_status == STONED) {

		return success(heal_char.get_current_hp());
	}

	if (old_status == DEAD) {

		const auto roll{_ctx.get_random(D100)};

		const auto threshold{50 + (3 * vitality)};

		if (roll > threshold) {

			heal_char.set_status(ASHES);

			_ctx.controller->set_text(
				"heal_results",
				std::format("{} {} {}",
							_ctx.get_string("TEMPLE_OOPS_DEAD_PREFIX"),
							heal_char.get_name(),
							_ctx.get_string("TEMPLE_OOPS_DEAD_SUFFIX")));

			return false;
		}

		return success(1);
	}

	if (old_status == ASHES) {

		const auto roll{_ctx.get_random(D100)};

		const auto threshold{40 + (3 * vitality)};

		if (roll > threshold) {

			heal_char.set_status(LOST);
			heal_char.set_current_hp(0);
			heal_char.set_location(TRAINING);

			_ctx.controller->set_text(
				"heal_results",
				std::format("{} {} {}",
							_ctx.get_string("TEMPLE_OOPS_ASHES_PREFIX"),
							heal_char.get_name(),
							_ctx.get_string("TEMPLE_OOPS_ASHES_SUFFIX")));

			return false;
		}

		return success(heal_char.get_max_hp());
	}

	return false;
}

auto Sorcery::Heal::stop() -> int {

	if (_heal_tick != 0) {
		SDL_RemoveTimer(_heal_tick);
		_heal_tick = 0;
	}

	_ctx.controller->unset_flag("heal_finished");
	_ctx.controller->go_to(Enums::Screen::TEMPLE);

	return 0;
}