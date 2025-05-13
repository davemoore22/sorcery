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

#include "modules/recovery.hpp"
#include "common/macro.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "types/character.hpp"
#include "types/game.hpp"

Sorcery::Recovery::Recovery(System *system, Display *display, UI *ui,
							Controller *controller)
	: _system{system},
	  _display{display},
	  _ui{ui},
	  _controller{controller} {

	_initialise();
};

auto Sorcery::Recovery::_initialise() -> bool {

	_controller->unset_flag("napping_finished");
	_controller->unset_flag("recuperating_finished");

	return true;
}

// Napping timer only ever runs once, thus 0 is returned which cancels it
auto Sorcery::Recovery::_callback_napping(Uint32, void *param) -> Uint32 {

	((Recovery *)param)->_controller->set_flag("napping_finished");

	return 0;
}

// Recuperating timer will return non-zero as long as there are hit points left
// to heal and there is enough gold, or if the recuperation is cancelled
auto Sorcery::Recovery::_callback_recuperating(Uint32, void *param) -> Uint32 {

	auto character{static_cast<Character *>(param)};

	auto can_continue = [](Character *character, int weekly_cost,
						   int hp_per_week) -> bool {
		auto current_hp{character->get_current_hp()};
		const auto max_hp{character->get_max_hp()};
		int current_gold{character->get_gold()};
		if ((current_hp < max_hp) && (current_gold >= weekly_cost)) {
			character->set_current_hp(current_hp + hp_per_week);
			character->set_gold(current_gold - weekly_cost);
			if (current_hp > max_hp)
				character->set_current_hp(max_hp);
			return true;
		} else
			return false;
	};

	switch (character->mode) {
	case RECOVERY_MODE_COST_10:
		if (!can_continue(character, 10, 1)) {
			character->mode = -1;
			return 0;
		}
		break;
	case RECOVERY_MODE_COST_50:
		if (!can_continue(character, 50, 3)) {
			character->mode = -1;
			return 0;
		}
		break;
	case RECOVERY_MODE_COST_200:
		if (!can_continue(character, 200, 5)) {
			character->mode = -1;
			return 0;
		}
		break;
	case RECOVERY_MODE_COST_500:
		if (!can_continue(character, 500, 10)) {
			character->mode = -1;
			return 0;
		}
		break;
	default:
		return 0;
	}

	const auto current_age{character->get_age()};
	character->set_age(current_age + 1);
	return 1000;
}

auto Sorcery::Recovery::start(Game *game, const int mode) -> int {

	_controller->move_screen("show_stay", "show_recovery");
	_controller->unset_flag("napping_finished");
	_controller->unset_flag("recuperating_finished");

	_character = &game->characters.at(_controller->get_character("stay"));
	_character->mode = mode;
	if (mode & RECOVERY_MODE_FREE)
		_rest_tick = SDL_AddTimer(1000, &Recovery::_callback_napping, this);
	else
		_rest_tick =
			SDL_AddTimer(1000, &Recovery::_callback_recuperating, _character);

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
				return BACK_TO_STAY;
		}

		_ui->display("recovery", game, mode);

		if (_character->mode == -1)
			_controller->set_flag("recuperating_finished");
		if (!_controller->has_flag("show_recovery"))
			return CHECK_FOR_LEVEL_GAIN;
		if (_controller->has_flag("napping_finished"))
			return CHECK_FOR_LEVEL_GAIN;
		if (_controller->has_flag("recuperating_finished"))
			return CHECK_FOR_LEVEL_GAIN;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Recovery::stop() -> int {

	SDL_RemoveTimer(_rest_tick);

	_controller->move_screen("show_recovery", "show_stay");

	return 0;
}