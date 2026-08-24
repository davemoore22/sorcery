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

#include "engine/chest.hpp"
#include "common/macro.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/debug.hpp"
#include "core/display.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "engine/define.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "gui/modal.hpp"
#include "gui/popup.hpp"
#include "types/character.hpp"
#include "types/game.hpp"
#include "types/meta.hpp"
#include "types/state.hpp"

Sorcery::Chest::Chest(Context &ctx)
	: Module{ctx} {

	_initialise();
};

Sorcery::Chest::~Chest() {}

auto Sorcery::Chest::_initialise() -> bool {

	_state.inspected.fill(false);

	return true;
}

auto Sorcery::Chest::start(void) -> Enums::Chests::Result {

	_ctx.controller->go_to(Enums::Screen::CHEST);
	_ctx.controller->initialise();

	_ctx.ui->modal_chest->show = false;
	_ctx.controller->unset_flag("want_chest");

	fade_in(Enums::Screen::CHEST, QUICK_FADE);

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
				return Enums::Chests::Result::LEFT;
		}

		_process_menu_action();
		_process_character_action();

		if (_ctx.controller->has_flag("chest_character_cancelled")) {

			_ctx.controller->unset_flag("chest_character_cancelled");
			_state.state = Enums::Chests::State::MENU;
		}

		_ctx.ui->display(Enums::Screen::CHEST,
						 std::to_underlying(_state.state));
		_ctx.tick();

		if (!_ctx.controller->wants(Enums::Screen::CHEST))
			return Enums::Chests::Result::LEFT;
	}

	// Exit if we get to here having broken out of the loop
	return Enums::Chests::Result::ABORT;
}

auto Sorcery::Chest::stop(void) -> void {

	fade_out(Enums::Screen::CHEST, QUICK_FADE);
}

auto Sorcery::Chest::_show_character_modal(const std::string_view menu_name)
	-> void {

	_ctx.ui->modal_chest->regenerate(menu_name);
	_ctx.ui->modal_chest->show = true;
}

auto Sorcery::Chest::_inspect(const int character_id) -> void {

	DEBUG_LOGF("INSPECT {}", character_id);

	const auto party{_ctx.game->state->get_party_characters()};

	const auto it{std::ranges::find(party, character_id)};

	if (it == party.end())
		return;

	const auto position{
		static_cast<std::size_t>(std::distance(party.begin(), it))};

	if (_state.inspected[position])
		return;

	_state.inspected[position] = true;

	// Roll inspection result...
}

auto Sorcery::Chest::_open(int character_id) -> void {

	DEBUG_LOGF("OPEN {}", character_id);
}

auto Sorcery::Chest::_cast_calfo(int character_id) -> void {

	DEBUG_LOGF("CALFO {}", character_id);
}

auto Sorcery::Chest::_disarm(int character_id, Enums::Traps::Type trap)
	-> void {

	DEBUG_LOGF("DISARM {}", character_id);
}

auto Sorcery::Chest::_trigger_trap(int character_id) -> void {}

auto Sorcery::Chest::_process_menu_action() -> void {

	if (!_ctx.controller->has_selected("chest_menu_action"))
		return;

	const auto action{_ctx.controller->get_selected("chest_menu_action")};

	_ctx.controller->unset_selected("chest_menu_action");

	using enum Enums::Chests::State;

	switch (action) {

	case 0: // Open
		_state.state = CHOOSE_OPEN_CHARACTER;
		_show_character_modal("chest_open_menu");
		break;

	case 1: // Inspect
		_state.state = CHOOSE_INSPECT_CHARACTER;
		_show_character_modal("chest_inspect_menu");
		break;

	case 2: // Calfo
		_state.state = CHOOSE_CALFO_CHARACTER;
		_show_character_modal("chest_calfo_menu");
		break;

	case 3: // Disarm
		_state.state = CHOOSE_DISARM_CHARACTER;
		_show_character_modal("chest_disarm_menu");
		break;

	default:
		_state.state = MENU;
		break;
	}
}

auto Sorcery::Chest::_process_character_action() -> void {

	if (!_ctx.controller->has_character(Enums::CharacterSlot::TRAP))
		return;

	const auto character_id{
		_ctx.controller->get_character(Enums::CharacterSlot::TRAP)};

	_ctx.controller->clear_character(Enums::CharacterSlot::TRAP);

	using enum Enums::Chests::State;

	switch (_state.state) {

	case CHOOSE_OPEN_CHARACTER:
		_open(character_id);
		_state.state = MENU;
		break;

	case CHOOSE_INSPECT_CHARACTER:
		_inspect(character_id);
		_state.state = MENU;
		break;

	case CHOOSE_CALFO_CHARACTER:
		_cast_calfo(character_id);
		_state.state = MENU;
		break;

	case CHOOSE_DISARM_CHARACTER:
		_state.actor = character_id;
		_state.state = CHOOSE_TRAP;
		break;

	default:
		break;
	}
}

auto Sorcery::Chest::_trap_name(const Enums::Traps::Type trap) const
	-> std::string {

	using enum Enums::Traps::Type;

	switch (trap) {

	case NONE:
		return _ctx.get_string("CHEST_NO_TRAP");

	case POISON_NEEDLE:
		return _ctx.get_string("CHEST_POISON_NEEDLE");

	case GAS_BOMB:
		return _ctx.get_string("CHEST_GAS_BOMB");

	case CROSSBOW_BOLT:
		return _ctx.get_string("CHEST_CROSSBOW_BOLT");

	case EXPLODING_BOX:
		return _ctx.get_string("CHEST_EXPLODING_BOX");

	case SPLINTERS:
		return _ctx.get_string("CHEST_SPLINTERS");

	case BLADES:
		return _ctx.get_string("CHEST_BLADES");

	case STUNNER:
		return _ctx.get_string("CHEST_STUNNER");

	case TELEPORTER:
		return _ctx.get_string("CHEST_TELEPORTER");

	case MAGE_BLASTER:
		return _ctx.get_string("CHEST_MAGE_BLASTER");

	case PRIEST_BLASTER:
		return _ctx.get_string("CHEST_PRIEST_BLASTER");

	case ALARM:
		return _ctx.get_string("CHEST_ALARM");
	}

	return {};
}