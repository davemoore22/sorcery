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

#include "modules/heal.hpp"
#include "common/macro.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "types/character.hpp"
#include "types/game.hpp"

Sorcery::Heal::Heal(System *system, Display *display, UI *ui,
					Controller *controller)
	: _system{system},
	  _display{display},
	  _ui{ui},
	  _controller{controller} {

	_initialise();
};

auto Sorcery::Heal::_initialise() -> bool {

	_controller->unset_flag("heal_finished");
	_stage = 5;
	_results = "";

	return true;
}

// Heal timer will return non-zero as long as there are stages left
auto Sorcery::Heal::_callback_heal_tick(Uint32, void *param) -> Uint32 {

	auto &current_stage{static_cast<Heal *>(param)->_stage};
	--current_stage;

	if (current_stage == 0)
		return 0;
	else
		return 2000;
}

auto Sorcery::Heal::start(Game *game) -> int {

	_controller->move_screen("show_pay", "show_heal");
	_controller->unset_flag("heal_finished");

	_heal_tick = SDL_AddTimer(2000, &Heal::_callback_heal_tick, this);

	_character = &game->characters.at(_controller->get_character("help"));

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			// Check for Quit Events
			ImGui_ImplSDL2_ProcessEvent(&event);
			done = _controller->check_for_abort(event);

			// Check for Window Resize
			_controller->check_for_resize(event, _ui);

			// Check for Back Event
			if (_controller->check_for_back(event))
				return BACK_TO_TEMPLE;
		}

		_ui->display("heal", game, _stage);

		if (_stage <= 0) {

			// Handle Healing
			_try_heal(game, _controller->get_character("help"),
					  _controller->get_character("pay"));
			_controller->set_flag("heal_finished");
		}

		if (!_controller->has_flag("show_heal"))
			return BACK_TO_TEMPLE;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

// If we get here we can afford to heal
auto Sorcery::Heal::_try_heal(Game *game, int heal_char_id, int pay_char_id)
	-> bool {

	using Enums::Character::CStatus;
	using Enums::Character::Location;
	using Enums::System::Random;

	auto &pay_char{game->characters[pay_char_id]};
	auto &heal_char{game->characters[heal_char_id]};

	auto results{""s};

	// Subtract money cost from selected character
	const auto cost{heal_char.get_cure_cost()};
	pay_char.grant_gold(0 - cost);

	if (heal_char.get_status() == CStatus::DEAD) {

		const auto chance{heal_char.get_ress_chance(false)};
		const auto roll((*_system->random)[Random::D100]);

		if (roll < chance) {

			results = fmt::format("{} {} {}",
								  (*_system->strings)["TEMPLE_HEALED_PREFIX"],
								  heal_char.get_name(),
								  (*_system->strings)["TEMPLE_HEALED_SUFFIX"]);
			heal_char.set_status(CStatus::OK);
			heal_char.set_current_hp(1);
			heal_char.set_location(Location::TAVERN);
			_controller->set_text("heal_results", results);

			return true;
		} else {

			results = fmt::format(
				"{} {} {}", (*_system->strings)["TEMPLE_OOPS_DEAD_PREFIX"],
				heal_char.get_name(),
				(*_system->strings)["TEMPLE_OOPS_DEAD_SUFFIX"]);
			heal_char.set_status(CStatus::ASHES);
			_controller->set_text("heal_results", results);

			return false;
		}

	} else if (heal_char.get_status() == CStatus::ASHES) {

		const auto chance{heal_char.get_ress_chance(false)};
		const auto roll((*_system->random)[Random::D100]);

		if (roll < chance) {

			results = fmt::format("{} {} {}",
								  (*_system->strings)["TEMPLE_HEALED_PREFIX"],
								  heal_char.get_name(),
								  (*_system->strings)["TEMPLE_HEALED_SUFFIX"]);
			heal_char.set_status(CStatus::OK);
			heal_char.set_current_hp(1);
			heal_char.set_location(Location::TAVERN);
			_controller->set_text("heal_results", results);

			return true;

		} else {

			results = fmt::format(
				"{} {} {}", (*_system->strings)["TEMPLE_OOPS_ASHES_PREFIX"],
				heal_char.get_name(),
				(*_system->strings)["TEMPLE_OOPS_ASHES_SUFFIX"]);
			heal_char.set_status(CStatus::LOST);
			heal_char.set_location(Location::TRAINING);
			heal_char.set_current_hp(0);
			_controller->set_text("heal_results", results);

			return false;
		}

	} else {

		results = fmt::format(
			"{} {} {}", (*_system->strings)["TEMPLE_HEALED_PREFIX"],
			heal_char.get_name(), (*_system->strings)["TEMPLE_HEALED_SUFFIX"]);
		heal_char.set_status(CStatus::OK);
		_controller->set_text("heal_results", results);

		return true;
	};
}

auto Sorcery::Heal::stop() -> int {

	SDL_RemoveTimer(_heal_tick);

	_controller->move_screen("show_heal", "show_title");

	return 0;
}