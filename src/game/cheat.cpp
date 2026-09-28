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

#include "game/cheat.hpp"
#include "common/enum.hpp"				  // for Class, Align, Random, Status
#include "core/context.hpp"				  // for Context
#include "core/controller/controller.hpp" // for Controller
#include "core/debug.hpp"				  // for DEBUG_LOG
#include "core/resources.hpp"			  // for Resources
#include "game/game.hpp"				  // for Game
#include "resources/itemstore.hpp"		  // for ItemStore
#include "types/character/character.hpp"  // for Character
#include "types/character/create.hpp"	  // for CharacterCreate
#include "types/character/inventory.hpp"  // for Inventory
#include "types/enum.hpp"				  // for TypeID, TypeID::LONG_SWORD
#include "types/item/item.hpp"			  // for Item
#include "types/meta.hpp"				  // for enum_cast
#include "types/state.hpp"				  // for State
#include <functional>					  // for invoke
#include <map>							  // for map
#include <memory>						  // for unique_ptr
#include <optional>						  // for optional
#include <vector>						  // for vector

namespace Sorcery {

const Cheat::Action Cheat::_actions[]{

	{"Create random party", &Cheat::create_random_party},
	{"Fill party with unidentified items", &Cheat::fill_party_unid_items},
	{"Give party gold", &Cheat::give_party_gold},
	{"Give party random items", &Cheat::give_party_random_items},
	{"Give party random status", &Cheat::give_party_random_status},
	{"Give party XP", &Cheat::give_party_xp},
	{"Heal party to full", &Cheat::heal_party_to_full},
	{"Harm party to minimum", &Cheat::harm_party_to_min},
	{"Level up all party members", &Cheat::level_up_party},
	{"Kill party", &Cheat::kill_party},
	{"Toggle LOMILWA", &Cheat::toggle_light},
	{"Toggle LATUMAPIC", &Cheat::toggle_identify},
	{"Toggle MAPORFIC", &Cheat::toggle_shield},
	{"Give party quest items", &Cheat::give_party_quest_items},
	{"Start chest event", &Cheat::start_chest_event},
	{"Toggle debug display", &Cheat::show_debug},
};

}

Sorcery::Cheat::Cheat(Context &ctx, Game &game)
	: _ctx{ctx},
	  _game{game} {}

auto Sorcery::Cheat::actions() const -> std::span<const Action> {

	return _actions;
}

auto Sorcery::Cheat::execute(const Action &action) -> void {

	std::invoke(action.handler, this);
}

auto Sorcery::Cheat::start_chest_event() -> void {

	DEBUG_LOG("debug_start_chest_event");

	_ctx.controller->set_flag("debug_start_chest");
}

auto Sorcery::Cheat::harm_party_to_min() -> void {

	DEBUG_LOG("debug_harm_party_to_min");

	for (const auto party{_game.state->get_party_characters()}; auto idx : party) {
		auto &cur_char{_game.characters.at(idx)};
		const auto hp{_ctx.get_random(Enums::System::Random::D4)};
		cur_char.set_current_hp(hp);
	}
}

auto Sorcery::Cheat::level_up_party() -> void {

	DEBUG_LOG("debug_level_up_party");

	for (const auto party{_game.state->get_party_characters()}; auto idx : party) {
		auto &cur_char{_game.characters.at(idx)};
		const auto xp_needed{cur_char.get_next_xp()};
		cur_char.grant_xp(xp_needed + 1);
		cur_char.create().level_up();
	}
}

auto Sorcery::Cheat::show_debug() -> void {

	DEBUG_LOG("debug_show_debug");
}

auto Sorcery::Cheat::give_party_quest_items() -> void {

	DEBUG_LOG("debug_give_party_quest_items");

	using enum Enums::Items::TypeID;

	_game.give_party_item(STATUE_OF_BEAR);
	_game.give_party_item(STATUE_OF_FROG);
	_game.give_party_item(KEY_OF_BRONZE);
	_game.give_party_item(KEY_OF_SILVER);
	_game.give_party_item(KEY_OF_GOLD);
	_game.give_party_item(BLUE_RIBBON);
	_game.give_party_item(AMULET_OF_WERDNA);
}

auto Sorcery::Cheat::kill_party() -> void {

	DEBUG_LOG("debug_kill_party");

	for (const auto party{_game.state->get_party_characters()}; auto idx : party) {
		auto &cur_char{_game.characters.at(idx)};
		cur_char.set_status(Enums::Character::Status::DEAD);
		cur_char.set_current_hp(0);
	}
}

auto Sorcery::Cheat::give_party_random_status() -> void {

	DEBUG_LOG("debug_give_party_random_status");

	for (const auto party{_game.state->get_party_characters()}; auto idx : party) {

		auto &cur_char{_game.characters.at(idx)};

		cur_char.set_status(
			enum_cast<Enums::Character::Status>(_ctx.get_random(Enums::System::Random::ZERO_TO_8)).value());
		using enum Enums::Character::Status;
		if ((cur_char.get_status() == DEAD) || (cur_char.get_status() == ASHES) || (cur_char.get_status() == LOST)) {
			cur_char.set_current_hp(0);
		} else
			cur_char.set_current_hp(cur_char.get_max_hp());
	}
}

auto Sorcery::Cheat::toggle_light() -> void {

	DEBUG_LOG("_debug_toggle_light");

	if (_game.state->get_lit())
		_game.state->set_lit(0);
	else
		_game.state->set_lit(32000);
};

auto Sorcery::Cheat::toggle_shield() -> void {

	DEBUG_LOG("_debug_toggle_shield");

	_game.state->set_maporfic(!_game.state->get_maporifc());
}

auto Sorcery::Cheat::toggle_identify() -> void {

	DEBUG_LOG("_debug_toggle_identify");

	_game.state->set_latumapic(!_game.state->get_latumapic());
}

auto Sorcery::Cheat::heal_party_to_full() -> void {

	DEBUG_LOG("debug_heal_party_to_full");

	for (const auto party{_game.state->get_party_characters()}; auto idx : party) {
		auto &cur_char{_game.characters.at(idx)};
		cur_char.set_status(Enums::Character::Status::OK);
		cur_char.set_current_hp(cur_char.get_max_hp());
		cur_char.reset_adjustment_per_turn();
		cur_char.set_poisoned_rate(0);
	}
}

auto Sorcery::Cheat::give_party_xp() -> void {

	DEBUG_LOG("debug_give_party_xp");

	for (const auto party{_game.state->get_party_characters()}; auto idx : party) {
		auto &cur_char{_game.characters.at(idx)};
		cur_char.grant_xp(cur_char.get_level() * 1000);
	}
}

auto Sorcery::Cheat::give_party_random_items() -> void {

	DEBUG_LOG("debug_give_party_random_items");

	for (const auto party{_game.state->get_party_characters()}; auto idx : party) {
		auto &cur_char{_game.characters.at(idx)};
		auto slots_free = cur_char.inventory.get_empty_slots();
		for (auto i = 0u; i < slots_free; i++) {
			using enum Enums::Items::TypeID;
			if (cur_char.inventory.get_empty_slots() > 0) {
				auto item{_ctx.resources->items->get_random_item(LONG_SWORD, RING_OF_DEATH)};
				cur_char.inventory.add(item);
			}
		}
	}
}

auto Sorcery::Cheat::give_party_gold() -> void {

	DEBUG_LOG("debug_give_party_gold");

	for (const auto party{_game.state->get_party_characters()}; auto idx : party) {
		auto &cur_char{_game.characters.at(idx)};
		cur_char.set_gold(10000);
	}
}

auto Sorcery::Cheat::create_random_party() -> void {

	// Clear any existing party
	_game.state->clear_party();

	DEBUG_LOG("debug_create_random_party");

	// Create a new random party of a random alignment
	const auto align{_ctx.get_random(Enums::System::Random::D2) == 1 ? Enums::Character::Align::GOOD
																	 : Enums::Character::Align::EVIL};
	for (int i = 0; i < 6; i++) {
		auto pc{Character(&_ctx)};
		switch (i) {
			using enum Enums::Character::Align;
			using enum Enums::Character::Class;
		case 0:
			pc.create().create_class_alignment(FIGHTER, align);
			break;
		case 1:
			pc.create().create_class_alignment(FIGHTER, NEUTRAL);
			break;
		case 2:
			pc.create().create_class_alignment(THIEF, NEUTRAL);
			break;
		case 3:
			pc.create().create_class_alignment(PRIEST, align);
			break;
		case 4:
			pc.create().create_class_alignment(BISHOP, align);
			break;
		case 5:
			pc.create().create_class_alignment(MAGE, NEUTRAL);
			break;
		default:
			return;
		}

		pc.create().finalise();
		pc.set_location(Enums::Character::Location::PARTY);
		pc.create().set_stage(Enums::Character::Stage::COMPLETED);
		pc.inventory.clear();

		switch (pc.get_class()) { // NOLINT(clang-diagnostic-switch)#
			using enum Enums::Items::TypeID;
			using enum Enums::Character::Class;
		case FIGHTER:
		case LORD:
		case SAMURAI:
			pc.inventory.add_type(_ctx.resources->items->get(LEATHER_ARMOR), true);
			pc.inventory.add_type(_ctx.resources->items->get(LONG_SWORD), true);
			break;
		case MAGE:
			pc.inventory.add_type(_ctx.resources->items->get(ROBES), true);
			pc.inventory.add_type(_ctx.resources->items->get(DAGGER), true);
			break;
		case PRIEST:
		case BISHOP:
			pc.inventory.add_type(_ctx.resources->items->get(ROBES), true);
			pc.inventory.add_type(_ctx.resources->items->get(STAFF), true);
			break;
		case THIEF:
		case NINJA:
			pc.inventory.add_type(_ctx.resources->items->get(LEATHER_ARMOR), true);
			pc.inventory.add_type(_ctx.resources->items->get(SHORT_SWORD), true);
		default:
			break;
		}

		auto char_id{_game.save_character(pc)};
		_game.characters[char_id] = pc;
		_game.state->add_character_to_party(char_id);
	}

	_game.save_game();
}

auto Sorcery::Cheat::fill_party_unid_items() -> void {

	DEBUG_LOG("debug_fill_party_unid_items");

	for (const auto party{_game.state->get_party_characters()}; auto idx : party) {
		auto &cur_char{_game.characters.at(idx)};
		auto slots_free = cur_char.inventory.get_empty_slots();
		for (auto i = 0u; i < slots_free; i++) {
			if (cur_char.inventory.get_empty_slots() > 0) {
				auto item{_ctx.resources->items->get_random_item(Enums::Items::TypeID::LONG_SWORD,
																 Enums::Items::TypeID::RING_OF_DEATH)};
				cur_char.inventory.add(item);
			}
		}
	}

	_game.save_game();
}