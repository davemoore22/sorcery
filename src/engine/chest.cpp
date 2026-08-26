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
#include "types/character/character.hpp"
#include "types/game.hpp"
#include "types/meta.hpp"
#include "types/state.hpp"

Sorcery::Chest::Chest(Context &ctx)
	: Module{ctx} {

	_initialise();
};

Sorcery::Chest::~Chest() {}

auto Sorcery::Chest::_initialise() -> bool {

	_state.state = Enums::Chests::State::MENU;
	_state.actor.reset();
	_state.inspected.fill(false);

	return true;
}
auto Sorcery::Chest::start(void) -> Enums::Chests::Result {

	_ctx.controller->go_to(Enums::Screen::CHEST);
	_ctx.controller->initialise();

	_ctx.controller->unset_selected("chest_menu_action");
	_ctx.controller->unset_selected("chest_trap_selection");
	_ctx.controller->unset_flag("chest_character_cancelled");
	_ctx.controller->unset_flag("chest_trap_cancelled");
	_ctx.controller->clear_character(Enums::CharacterSlot::TRAP);

	_initialise();

	_ctx.ui->modal_chest->show = false;
	_ctx.controller->unset_flag("want_chest");

	fade_in(Enums::Screen::CHEST, QUICK_FADE);

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

			if (!_ctx.ui->transient_blocks_input() &&
				_ctx.controller->check_for_back(event)) {

				return Enums::Chests::Result::LEFT;
			}
		}

		if (!_ctx.ui->transient_blocks_input()) {

			_process_menu_action();
			_process_character_action();
			_process_trap_action();

			if (_ctx.controller->has_flag("chest_character_cancelled")) {

				_ctx.controller->unset_flag("chest_character_cancelled");
				_state.state = Enums::Chests::State::MENU;
			}
		}

		_ctx.ui->display(Enums::Screen::CHEST,
						 std::to_underlying(_state.state));

		_ctx.tick();

		//
		// Chest has been resolved. Allow any result transient
		// to finish before returning to the engine.
		//
		if (_state.state == Enums::Chests::State::DONE &&
			!_ctx.ui->has_transient()) {

			return Enums::Chests::Result::OPENED;
		}

		if (!_ctx.controller->wants(Enums::Screen::CHEST))
			return Enums::Chests::Result::LEFT;
	}

	return Enums::Chests::Result::ABORT;
}

auto Sorcery::Chest::_show_trap_modal() -> void {

	_ctx.ui->modal_chest->set_title("CHEST_TRAP_TITLE");
	_ctx.ui->modal_chest->regenerate("chest_trap_menu");
	_ctx.ui->modal_chest->show = true;
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

	using namespace std::chrono_literals;
	using enum Enums::Character::Attribute;

	const auto party{_ctx.game->state->get_party_characters()};
	const auto it{std::ranges::find(party, character_id)};

	if (it == party.end())
		return;

	const auto position{
		static_cast<std::size_t>(std::distance(party.begin(), it))};

	const auto &character{_ctx.game->characters.at(character_id)};

	if (_state.inspected[position]) {

		_ctx.ui->show_transient(
			std::format("{}{}", character.get_name(),
						_ctx.get_string("CHEST_ALREADY_INSPECTED")),
			1s, TransientWidth::FIT_TEXT, TransientMode::UNTIL_EXPIRY);

		_state.state = Enums::Chests::State::MENU;
		return;
	}

	_state.inspected[position] = true;

	const int chance{character.get_identify_trap()};

	const int identify_roll{_ctx.get_random(Enums::System::Random::D100) - 1};

	std::string message;

	if (identify_roll < chance) {

		// Correct identification.
		message = _trap_name(_state.actual_trap);

	} else {

		const auto agility{character.get_cur_attr(AGILITY)};

		const auto trigger_roll{_ctx.get_random(Enums::System::Random::D20) -
								1};

		if (trigger_roll > agility) {

			_trigger_trap(character_id);
			_state.state = Enums::Chests::State::DONE;
			return;

		} else {

			// Failed identification, but no trigger:
			// display a random false result.
			message = _trap_name(_random_trap());
		}
	}

	_ctx.ui->show_transient(std::move(message), 1s, TransientWidth::FIT_TEXT,
							TransientMode::UNTIL_EXPIRY);

	_state.state = Enums::Chests::State::MENU;
}

auto Sorcery::Chest::_open(const int character_id) -> void {

	using namespace std::chrono_literals;

	const auto &character{_ctx.game->characters.at(character_id)};

	//
	// Trapless chest: opens safely.
	//
	if (_state.actual_trap == Enums::Traps::Type::NONE) {

		_ctx.ui->show_transient(_ctx.get_string("CHEST_NO_TRAP"), 1s,
								TransientWidth::FIT_TEXT,
								TransientMode::UNTIL_EXPIRY);

		_state.state = Enums::Chests::State::DONE;
		return;
	}

	//
	// Original OPENCHST:
	//
	//   (RANDOM MOD 1000) < CHARLEV
	//
	const int roll{_ctx.get_random(Enums::System::Random::D1000) - 1};

	if (roll < character.get_level()) {

		_state.state = Enums::Chests::State::DONE;
		return;
	}

	//
	// Failed to open safely: trigger the actual trap.
	//
	_trigger_trap(character_id);

	_state.state = Enums::Chests::State::DONE;
}

auto Sorcery::Chest::_cast_calfo(const int character_id) -> void {

	using namespace std::chrono_literals;

	const auto &character{_ctx.game->characters.at(character_id)};

	// Still validate here even though the menu should prevent
	// an invalid character being selected.
	if (character.magic().get_calfo_uses_left() == 0) {
		_state.state = Enums::Chests::State::MENU;
		return;
	}

	// Consume one level-2 priest spell slot.
	// (TODO: we need a mutable character for this!)

	const auto roll{_ctx.get_random(Enums::System::Random::D100) - 1};

	const auto trap{roll < 95 ? _state.actual_trap : _random_trap()};

	_ctx.ui->show_transient(_trap_name(trap), 1s, TransientWidth::FIT_TEXT,
							TransientMode::UNTIL_EXPIRY);

	_state.state = Enums::Chests::State::MENU;
}

auto Sorcery::Chest::_disarm(const int character_id,
							 const Enums::Traps::Type trap) -> void {

	using namespace std::chrono_literals;
	using enum Enums::Character::Attribute;
	using enum Enums::Character::Class;

	auto &character{_ctx.game->characters.at(character_id)};

	// Wrong trap guess: trigger the actual trap immediately.
	if (trap != _state.actual_trap) {

		_trigger_trap(character_id);
		_state.state = Enums::Chests::State::DONE;
		return;
	}

	const auto class_bonus{
		(character.get_class() == THIEF || character.get_class() == NINJA) ? 50
																		   : 0};

	const int chance{character.get_level() -
					 std::abs(_ctx.game->state->get_depth()) + class_bonus};

	const int disarm_roll{_ctx.get_random(Enums::System::Random::D70) - 1};

	if (disarm_roll < chance) {

		_ctx.ui->show_transient(_ctx.get_string("CHEST_DISARMED"), 1s,
								TransientWidth::FIT_TEXT,
								TransientMode::UNTIL_EXPIRY);

		_state.state = Enums::Chests::State::DONE;
		return;
	}

	const auto trigger_roll{_ctx.get_random(Enums::System::Random::D20) - 1};

	if (trigger_roll < character.get_cur_attr(AGILITY)) {

		_ctx.ui->show_transient(_ctx.get_string("CHEST_DISARM_FAILED"), 1s,
								TransientWidth::FIT_TEXT,
								TransientMode::UNTIL_EXPIRY);

		_state.state = Enums::Chests::State::MENU;
		return;
	}

	_trigger_trap(character_id);
	_state.state = Enums::Chests::State::DONE;
}

auto Sorcery::Chest::_trigger_trap(const int character_id) -> void {

	using namespace std::chrono_literals;
	using enum Enums::Traps::Type;

	_ctx.ui->show_transient(std::format("{}{}", _ctx.get_string("CHEST_OOPS"),
										_trap_name(_state.actual_trap)),
							1s, TransientWidth::FIT_TEXT,
							TransientMode::UNTIL_EXPIRY);

	switch (_state.actual_trap) {

	case NONE:
		break;

	case POISON_NEEDLE:
		// TODO
		break;

	case GAS_BOMB:
		// TODO
		break;

	case CROSSBOW_BOLT:
		// TODO
		break;

	case EXPLODING_BOX:
		// TODO
		break;

	case SPLINTERS:
		// TODO
		break;

	case BLADES:
		// TODO
		break;

	case STUNNER:
		// TODO
		break;

	case TELEPORTER:
		// TODO
		break;

	case MAGE_BLASTER:
		// TODO
		break;

	case PRIEST_BLASTER:
		// TODO
		break;

	case ALARM:
		// TODO
		break;
	}
}

auto Sorcery::Chest::_process_menu_action() -> void {

	if (!_ctx.controller->has_selected("chest_menu_action"))
		return;

	const auto action{_ctx.controller->get_selected("chest_menu_action")};

	_ctx.controller->unset_selected("chest_menu_action");

	using enum Enums::Chests::State;

	switch (action) {

	case 0: // Open
		_state.state = CHOOSE_OPEN_CHARACTER;
		_ctx.ui->modal_chest->set_title("CHEST_OPEN_TITLE");
		_show_character_modal("chest_open_menu");
		break;

	case 1: // Inspect
		_state.state = CHOOSE_INSPECT_CHARACTER;
		_ctx.ui->modal_chest->set_title("CHEST_INSPECT_TITLE");
		_show_character_modal("chest_inspect_menu");
		break;

	case 2: // Calfo
		_state.state = CHOOSE_CALFO_CHARACTER;
		_ctx.ui->modal_chest->set_title("CHEST_CALFO_TITLE");
		_show_character_modal("chest_calfo_menu");
		break;

	case 3: // Disarm
		_state.state = CHOOSE_DISARM_CHARACTER;
		_ctx.ui->modal_chest->set_title("CHEST_DISARM_TITLE");
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
		break;

	case CHOOSE_INSPECT_CHARACTER:
		_inspect(character_id);
		break;

	case CHOOSE_CALFO_CHARACTER:
		_cast_calfo(character_id);
		break;

	case CHOOSE_DISARM_CHARACTER:
		_state.actor = character_id;
		_state.state = CHOOSE_TRAP;
		_show_trap_modal();
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

auto Sorcery::Chest::_random_trap() const -> Enums::Traps::Type {

	using enum Enums::Traps::Type;

	const auto trap{_ctx.get_random(Enums::System::Random::D50) - 1};

	switch (trap % 8) {

	case 0:
		return NONE;

	case 1:
		return POISON_NEEDLE;

	case 2:
		return GAS_BOMB;

	case 3:
		switch (_ctx.get_random(Enums::System::Random::D5) - 1) {
		case 0:
			return CROSSBOW_BOLT;
		case 1:
			return EXPLODING_BOX;
		case 2:
			return SPLINTERS;
		case 3:
			return BLADES;
		case 4:
			return STUNNER;
		default:
			std::unreachable();
		}

	case 4:
		return TELEPORTER;

	case 5:
		return MAGE_BLASTER;

	case 6:
		return PRIEST_BLASTER;

	case 7:
		return ALARM;

	default:
		std::unreachable();
	}
}

auto Sorcery::Chest::_process_trap_action() -> void {

	using enum Enums::Traps::Type;
	using enum Enums::Chests::State;

	if (_ctx.controller->has_flag("chest_trap_cancelled")) {

		_ctx.controller->unset_flag("chest_trap_cancelled");
		_state.actor.reset();
		_state.state = MENU;
		return;
	}

	if (!_ctx.controller->has_selected("chest_trap_selection"))
		return;

	const auto selection{_ctx.controller->get_selected("chest_trap_selection")};

	_ctx.controller->unset_selected("chest_trap_selection");

	constexpr std::array traps{POISON_NEEDLE,  GAS_BOMB,   CROSSBOW_BOLT,
							   EXPLODING_BOX,  SPLINTERS,  BLADES,
							   STUNNER,		   TELEPORTER, MAGE_BLASTER,
							   PRIEST_BLASTER, ALARM};

	if (selection < 0 || selection >= static_cast<int>(traps.size())) {

		_state.state = MENU;
		_state.actor.reset();
		return;
	}

	if (!_state.actor) {
		_state.state = MENU;
		return;
	}

	const auto trap{traps.at(static_cast<std::size_t>(selection))};

	const auto character_id{*_state.actor};
	_state.actor.reset();

	_disarm(character_id, trap);
}