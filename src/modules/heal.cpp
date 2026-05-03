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
#include "common/macro.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "types/character.hpp"
#include "types/game.hpp"

Sorcery::Heal::Heal(Context &ctx)
	: _ctx{ctx} {

	_initialise();
};

auto Sorcery::Heal::_initialise() -> bool {

	_ctx.controller->unset_flag("heal_finished");
	_stage = 5;
	_results = "";

	return true;
}

// Heal timer will return non-zero as long as there are stages left
auto Sorcery::Heal::_callback_heal_tick(std::uint32_t, void *param)
	-> std::uint32_t {

	auto &current_stage{static_cast<Heal *>(param)->_stage};
	--current_stage;

	if (current_stage == 0)
		return 0;
	else
		return 2000;
}

auto Sorcery::Heal::start() -> int {

	_ctx.controller->go_to(Enums::Screen::HEAL);
	_ctx.controller->unset_flag("heal_finished");

	_heal_tick = SDL_AddTimer(2000, &Heal::_callback_heal_tick, this);

	_character =
		&_ctx.game->characters.at(_ctx.controller->get_character("help"));

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
				return BACK_TO_TEMPLE;
		}

		_ctx.ui->display(Enums::Screen::HEAL, _stage);
		_ctx.tick();

		if (_stage <= 0) {

			// Handle Healing
			_try_heal(_ctx.controller->get_character("help"),
					  _ctx.controller->get_character("pay"));
			_ctx.controller->set_flag("heal_finished");
		}

		if (!_ctx.controller->wants(Enums::Screen::HEAL))
			return BACK_TO_TEMPLE;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

// If we get here we can afford to heal
auto Sorcery::Heal::_try_heal(int heal_char_id, int pay_char_id) -> bool {

	auto &pay_char{_ctx.game->characters[pay_char_id]};
	auto &heal_char{_ctx.game->characters[heal_char_id]};

	auto results{""s};

	// Subtract money cost from selected character
	const auto cost{heal_char.get_cure_cost()};
	pay_char.grant_gold(0 - cost);

	using enum Enums::Character::Status;
	using enum Enums::Character::Location;
	using enum Enums::System::Random;
	if (heal_char.get_status() == DEAD) {

		const auto chance{heal_char.get_ress_chance(false)};
		const auto roll(_ctx.get_random(D100));

		if (roll < chance) {

			results = std::format(
				"{} {} {}", _ctx.get_string("TEMPLE_HEALED_PREFIX"),
				heal_char.get_name(), _ctx.get_string("TEMPLE_HEALED_SUFFIX"));
			heal_char.set_status(OK);
			heal_char.set_current_hp(1);
			heal_char.set_location(TAVERN);
			_ctx.controller->set_text("heal_results", results);

			return true;
		} else {

			results = std::format("{} {} {}",
								  _ctx.get_string("TEMPLE_OOPS_DEAD_PREFIX"),
								  heal_char.get_name(),
								  _ctx.get_string("TEMPLE_OOPS_DEAD_SUFFIX"));
			heal_char.set_status(ASHES);
			_ctx.controller->set_text("heal_results", results);

			return false;
		}

	} else if (heal_char.get_status() == ASHES) {

		const auto chance{heal_char.get_ress_chance(false)};
		const auto roll(_ctx.get_random(D100));

		if (roll < chance) {

			results = std::format(
				"{} {} {}", _ctx.get_string("TEMPLE_HEALED_PREFIX"),
				heal_char.get_name(), _ctx.get_string("TEMPLE_HEALED_SUFFIX"));
			heal_char.set_status(OK);
			heal_char.set_current_hp(1);
			heal_char.set_location(TAVERN);
			_ctx.controller->set_text("heal_results", results);

			return true;

		} else {

			results = std::format("{} {} {}",
								  _ctx.get_string("TEMPLE_OOPS_ASHES_PREFIX"),
								  heal_char.get_name(),
								  _ctx.get_string("TEMPLE_OOPS_ASHES_SUFFIX"));
			heal_char.set_status(LOST);
			heal_char.set_location(TRAINING);
			heal_char.set_current_hp(0);
			_ctx.controller->set_text("heal_results", results);

			return false;
		}

	} else {

		results = std::format(
			"{} {} {}", _ctx.get_string("TEMPLE_HEALED_PREFIX"),
			heal_char.get_name(), _ctx.get_string("TEMPLE_HEALED_SUFFIX"));
		heal_char.set_status(OK);
		_ctx.controller->set_text("heal_results", results);

		return true;
	};
}

auto Sorcery::Heal::stop() -> int {

	SDL_RemoveTimer(_heal_tick);

	_ctx.controller->go_to(Enums::Screen::TEMPLE);

	return 0;
}