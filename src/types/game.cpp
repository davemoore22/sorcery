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

#include "types/game.hpp"
#include "common/macro.hpp"
#include "core/resources.hpp"
#include "core/system.hpp"
#include "types/state.hpp"

Sorcery::Game::Game(System *system, Resources *resources)
	: _system{system},
	  _resources{resources} {

	if (_system->db->has_game()) {
		_clear();
		_load_game();
	} else {
		_create_game();
		_load_game();
	}

	_set_up_dungeon_events();
	_set_up_debug_keys();
}

auto Sorcery::Game::_set_up_debug_keys() -> void {

	_debug.clear();
	_debug[SDLK_F1] = std::bind(&Game::_debug_create_random_party, this);
	_debug[SDLK_F2] = std::bind(&Game::_debug_give_party_random_status, this);
	_debug[SDLK_F3] = std::bind(&Game::_debug_heal_party_to_full, this);
	_debug[SDLK_F4] = std::bind(&Game::_debug_toggle_light, this);
	_debug[SDLK_F5] = std::bind(&Game::_debug_harm_party_to_min, this);
	_debug[SDLK_F6] = std::bind(&Game::_debug_give_party_gold, this);
	_debug[SDLK_F7] = std::bind(&Game::_debug_give_party_xp, this);
	_debug[SDLK_F8] = std::bind(&Game::_debug_give_party_random_items, this);
}

// Call the Debug Function associated with the Key Press (if any)
auto Sorcery::Game::call_debug(const SDL_Keycode key_code) -> void {

	if (_debug.find(key_code) != _debug.end())
		_debug[key_code]();
}

auto Sorcery::Game::reset() -> void {

	_create_game();
	_load_game();
}

auto Sorcery::Game::wipe_data() -> void {

	_system->db->wipe_data();
}

auto Sorcery::Game::move_party_to_tavern() -> void {

	for (auto &[character_id, character] : characters) {
		if (character.get_location() == Enums::Character::Location::PARTY)
			character.set_location(Enums::Character::Location::TAVERN);
	}
	state->clear_party();
}

// Mappings for each type of special dungeon event in the Proving Grounds
auto Sorcery::Game::_set_up_dungeon_events() -> void {
	_events.clear();

	using Enums::Map::Event;

	// bool params are:
	//
	// search after
	// encounter after
	// go back a square after
	// teleport back to town

	// Level 1
	_events.emplace_back(Event::AREA_OF_OUT_BOUNDS, "event_area_out_of_bounds",
						 false, false, false, false);
	_events.emplace_back(Event::MAN_TELEPORT_CASTLE,
						 "event_man_teleport_castle", false, false, false,
						 true);
	_events.emplace_back(Event::SILVER_KEY, "event_silver_key", true, false,
						 false, false);
	_events.emplace_back(Event::BRONZE_KEY, "event_bronze_key", true, false,
						 false, false);
	_events.emplace_back(Event::MURPHYS_GHOSTS, "event_murphys_ghosts", true,
						 true, false, false);

	// Level 2
	_events.emplace_back(Event::GOLD_KEY, "event_gold_key", true, false, false,
						 false);
	_events.emplace_back(Event::BEAR_STATUE, "event_bear_statue", true, false,
						 false, false);
	_events.emplace_back(Event::FROG_STATUE, "event_frog_statue", true, false,
						 false, false);
	_events.emplace_back(Event::PLACARD_PIT_1, "event_placard_pit_1", false,
						 false, false, false);
	_events.emplace_back(Event::PLACARD_PIT_2, "event_placard_pit_2", false,
						 false, false, false);
	_events.emplace_back(Event::PLACARD_PIT_3, "event_placard_pit_3", false,
						 false, false, false);
	_events.emplace_back(Event::NEED_SILVER_KEY, "event_need_silver_key", false,
						 false, true, false);
	_events.emplace_back(Event::NEED_BRONZE_KEY, "event_need_bronze_key", false,
						 false, true, false);
	_events.emplace_back(Event::NEED_BEAR_STATUE,
						 "event_cannot_break_doors_down", false, false, true,
						 false);
	_events.emplace_back(Event::NEED_FROG_STATUE,
						 "event_cannot_break_doors_down", false, false, true,
						 false);

	// Level 3
	_events.emplace_back(Event::TURN_LEFT, "event_turn_left", false, false,
						 false, false);
	_events.emplace_back(Event::TURN_AROUND, "event_turn_around", false, false,
						 false, false);
	_events.emplace_back(Event::TURN_RIGHT, "event_turn_right", false, false,
						 false, false);

	// Level 4
	_events.emplace_back(Event::NEED_BEAR_STATUE_2, "event_need_bear_statue_2",
						 false, false, true, false);
	_events.emplace_back(Event::TESTING_GROUNDS, "event_testing_grounds", false,
						 false, false, false);
	_events.emplace_back(Event::ALARM_BELLS, "event_alarm_bells", false, true,
						 false, false);
	_events.emplace_back(Event::TREASURE_REPOSITORY,
						 "event_treasure_repository", false, false, false,
						 false);
	_events.emplace_back(Event::MONSTER_ALLOCATION_CENTRE,
						 "event_monster_allocation_centre", false, false, false,
						 false);
	_events.emplace_back(Event::NEED_BLUE_RIBBON, "event_need_blue_ribbon",
						 false, false, true, false);
	_events.emplace_back(Event::SERVICE_ELEVATOR, "event_service_elevator",
						 false, false, false, false);
	_events.emplace_back(Event::GUARANTEED_COMBAT, "event_guaranteed_combat",
						 false, true, false, false);
	_events.emplace_back(Event::LARGE_DESK, "event_large_desk", false, false,
						 false, false);
	_events.emplace_back(Event::DEADLY_RING_COMBAT, "event_deadly_ring_combat",
						 false, true, false, false);
	_events.emplace_back(Event::TREBOR_VOICE, "event_trebor_voice", false,
						 false, false, false);

	// Level 6
	_events.emplace_back(Event::THREE_HUMANOIDS, "event_three_humanoids", false,
						 false, false, false);

	// Level 7
	_events.emplace_back(Event::GETTING_WARM_1, "event_getting_warm_1", false,
						 false, false, false);
	_events.emplace_back(Event::GETTING_WARM_2, "event_getting_warm_2", false,
						 false, false, false);
	_events.emplace_back(Event::GETTING_WARM_3, "event_getting_warm_3", false,
						 false, false, false);
	_events.emplace_back(Event::FIRE_DRAGONS_COMBAT,
						 "event_fire_dragons_combat", false, true, false,
						 false);

	// Level 10
	_events.emplace_back(Event::WERDNA_BOAST, "event_werdna_boast", false,
						 false, false, false);
	_events.emplace_back(Event::TURN_BACK, "event_turn_back", false, false,
						 false, false);
	_events.emplace_back(Event::WERDNA_SIGN, "event_werdna_sign_1", false,
						 false, false, false);
}

auto Sorcery::Game::get_event(Enums::Map::Event event_type) const
	-> DungeonEvent {

	auto it{std::ranges::find_if(_events.begin(), _events.end(),
								 [&](const auto &dungeon_event) {
									 return (dungeon_event.event == event_type);
								 })};

	return *it;

	// TODO: handle updating of these
}

auto Sorcery::Game::enable_event(Enums::Map::Event event_type) -> void {

	auto it{std::ranges::find_if(_events.begin(), _events.end(),
								 [&](const auto &dungeon_event) {
									 return (dungeon_event.event == event_type);
								 })};

	if (it != _events.end())
		(*it).enabled = true;
}

auto Sorcery::Game::disable_event(Enums::Map::Event event_type) -> void {

	auto it{std::ranges::find_if(_events.begin(), _events.end(),
								 [&](const auto &dungeon_event) {
									 return (dungeon_event.event == event_type);
								 })};

	if (it != _events.end())
		(*it).enabled = false;
}

auto Sorcery::Game::get_id() const -> unsigned int {

	return _id;
}

auto Sorcery::Game::create_game() -> void {

	_create_game();
}

auto Sorcery::Game::load_game() -> void {

	_load_game();
}

auto Sorcery::Game::save_game() -> void {

	_save_game();
}

auto Sorcery::Game::enter_maze() -> void {

	Level level{((*levels)[-1]).value()};
	state->set_current_level(&level);
	state->restart_expedition();
}

auto Sorcery::Game::restart_maze(unsigned int char_id) -> void {

	// Find the location and floor of the character pointed to, and reload the
	// maze, repopulate the party and restart the game from there
	auto &character{characters[char_id]};
	const auto to_depth{character.depth.value()};
	const auto to_loc{character.coordinate.value()};
	state->clear_party();
	for (auto &[id, character] : characters) {
		if (character.get_location() == Enums::Character::Location::MAZE) {
			if (character.depth.value() == to_depth &&
				character.coordinate.value() == to_loc) {
				character.set_location(Enums::Character::Location::PARTY);
				state->add_character_by_id(id);
			}
		}
	}

	state->set_depth(to_depth);
	state->set_player_prev_depth(state->get_depth());
	state->set_player_pos(to_loc);
	Level level{((*levels)[to_depth]).value()};
	state->set_current_level(&level);
}

auto Sorcery::Game::delete_character(unsigned int char_id) -> void {

	_system->db->delete_character(_id, char_id);
}

auto Sorcery::Game::_clear() -> void {

	// Clear existing data!
	if (state.get()) {
		state.release();
		state.reset();
	}
	if (levels.get()) {
		levels.release();
		levels.reset();
	}

	characters.clear();
	_char_ids.clear();
	state = std::make_unique<State>(_system);
	levels =
		std::make_unique<LevelStore>(_system, (*_system->files)[MAPS_FILE]);

	state->clear_log_messages();
	state->reset_shop(_resources->items.get());

	state->level->reset();
}

auto Sorcery::Game::_create_game() -> void {
	_clear();

	std::stringstream ss;
	{
		cereal::XMLOutputArchive out_archive(ss);
		state->add_log_message("New Game Started",
							   Enums::Internal::MessageType::GAME);
		out_archive(state);
	}
	const auto data{ss.str()};
	_id = _system->db->create_game_state(data);
}

auto Sorcery::Game::_load_game() -> void {

	// Get Game and State Data
	auto [id, key, status, start_time, last_time, data] =
		_system->db->load_game_state().value();
	_id = id;
	_key = key;
	_status = status;
	_start_time = start_time;
	_last_time = last_time;
	state = std::make_unique<State>();
	levels =
		std::make_unique<LevelStore>(_system, (*_system->files)[MAPS_FILE]);
	if (data.length() > 0) {
		std::stringstream ss;
		ss.str(data);
		{
			cereal::XMLInputArchive in_archive(ss);
			in_archive(state);
			state->set(_system);
		}
	}

	// And load the associated characters
	_load_characters();
}

auto Sorcery::Game::pass_turn(unsigned int turns) -> void {

	state->pass_turn(turns);
}

auto Sorcery::Game::get_turns() const -> unsigned int {

	return state->get_turns();
}

auto Sorcery::Game::show_console() -> void {

	_show_console = true;
}

auto Sorcery::Game::hide_console() -> void {

	_show_console = false;
}

auto Sorcery::Game::toggle_console() -> void {

	_show_console = !_show_console;
}

auto Sorcery::Game::get_console_status() const -> bool {

	return _show_console;
}

auto Sorcery::Game::_save_game() -> void {

	std::stringstream ss;
	{
		cereal::XMLOutputArchive archive(ss);
		archive(state);
	}
	auto data{ss.str()};

	_system->db->save_game_state(_id, _key, data);
	_save_characters();
}

auto Sorcery::Game::_save_characters() -> void {
	for (const auto &[char_id, character] : characters) {

		_update_party_location();
		std::stringstream ss;
		{
			cereal::XMLOutputArchive out_archive(ss);
			out_archive(character);
		}
		const auto char_data{ss.str()};

		_system->db->update_character(_id, char_id, character.get_name(),
									  char_data);
	}
}

auto Sorcery::Game::has_party_in_maze() const -> bool {

	for (auto &[char_id, character] : characters) {

		if (character.get_location() == Enums::Character::Location::MAZE)
			return true;
	}

	return false;
}

auto Sorcery::Game::_update_party_location() -> void {

	auto party{state->get_party_characters()};
	for (auto &[char_id, character] : characters) {
		if (std::find(party.begin(), party.end(), char_id) != party.end()) {
			character.depth = state->get_depth();
			character.coordinate = state->get_player_pos();
		}
	}
}

auto Sorcery::Game::get_characters_at_loc() const -> std::vector<unsigned int> {

	std::vector<unsigned int> results;
	for (auto &[char_id, character] : characters) {
		if (character.get_location() == Enums::Character::Location::MAZE) {
			if (character.coordinate == state->get_player_pos()) {
				results.emplace_back(char_id);
			}
		}
	}

	return results;
}

auto Sorcery::Game::save_character(Character character) -> unsigned int {

	std::stringstream ss;
	{
		cereal::XMLOutputArchive out_archive(ss);
		out_archive(character);
	}
	const auto char_data{ss.str()};

	return _system->db->add_character(_id, character.get_name(), char_data);
}

auto Sorcery::Game::update_character(unsigned int game_id, unsigned int char_id,
									 Character &character) -> bool {

	std::stringstream ss;
	{
		cereal::XMLOutputArchive out_archive(ss);
		out_archive(character);
	}
	const auto character_data{ss.str()};

	return _system->db->update_character(game_id, char_id, character.get_name(),
										 character_data);
}

auto Sorcery::Game::_load_characters() -> void {

	_char_ids.clear();
	_char_ids = _system->db->get_character_ids(_id);
	characters.clear();

	for (auto char_id : _char_ids) {

		const auto data{_system->db->get_character(_id, char_id)};
		std::stringstream ss;
		ss.str(data);

		Character character(_system, _resources);
		{
			cereal::XMLInputArchive in_archive(ss);
			in_archive(character);
		}
		character.create_spells();
		character.set_spells();
		characters[char_id] = character;
	}
}

auto Sorcery::Game::get_party_alignment() const -> Enums::Character::Align {

	using Enums::Character::Align;

	const auto party{state->get_party_characters()};
	auto good{0u}, evil{0u};

	for (auto idx : party) {
		auto cur_char{&characters.at(idx)};
		if (cur_char->get_alignment() == Align::GOOD)
			++good;
		else if (cur_char->get_alignment() == Align::EVIL)
			++evil;
	}

	if (good > 0)
		return Align::GOOD;
	else if (evil > 0)
		return Align::EVIL;
	else
		return Align::NEUTRAL;
}

auto Sorcery::Game::divvy_party_gold() -> void {

	const auto party{state->get_party_characters()};
	auto gold{0};

	for (auto idx : party) {
		auto cur_char{&characters.at(idx)};
		gold += cur_char->get_gold();
	}

	gold = gold / party.size();
	for (auto idx : party) {
		auto cur_char{&characters.at(idx)};
		cur_char->set_gold(gold);
	}
}

auto Sorcery::Game::pool_party_gold(unsigned int char_id) -> void {

	const auto party{state->get_party_characters()};
	auto gold{0};

	for (auto idx : party) {
		auto cur_char{&characters.at(idx)};
		gold += cur_char->get_gold();
	}

	for (auto idx : party) {
		auto cur_char{&characters.at(idx)};
		if (char_id != idx)
			cur_char->set_gold(0);
		else
			cur_char->set_gold(gold);
	}
}

auto Sorcery::Game::print() -> void {

	auto text{"Game:\n\n"s};

	for (const auto &[char_id, character] : characters) {
		auto line{fmt::format("{}){:>16} {}", char_id, character.get_name(),
							  magic_enum::enum_name(character.get_location()))};
		text.append(line);
		text.append("\n");
	}

	text.append("\n[");
	for (const auto char_id : state->get_party_characters()) {
		auto line{fmt::format("{},", char_id)};
		text.append(line);
	}
	text.append("]\n");
	std::cout << text << std::endl;
}

auto Sorcery::Game::_debug_harm_party_to_min() -> void {

	using Enums::System::Random;

	PRINT("debug_harm_party_to_min");

	const auto party{state->get_party_characters()};
	for (auto idx : party) {
		auto &cur_char{characters.at(idx)};
		const auto hp{(*_system->random)[Random::D4]};
		cur_char.set_current_hp(hp);
	}
}

auto Sorcery::Game::_debug_give_party_random_status() -> void {

	using Enums::Character::CStatus;
	using Enums::System::Random;

	PRINT("debug_give_party_random_status");

	const auto party{state->get_party_characters()};
	for (auto idx : party) {
		auto &cur_char{characters.at(idx)};

		cur_char.set_status(magic_enum::enum_cast<CStatus>(
								(*_system->random)[Random::ZERO_TO_8])
								.value());
		if ((cur_char.get_status() == CStatus::DEAD) ||
			(cur_char.get_status() == CStatus::ASHES) ||
			(cur_char.get_status() == CStatus::LOST)) {
			cur_char.set_current_hp(0);
		} else
			cur_char.set_current_hp(cur_char.get_max_hp());
	}
}

auto Sorcery::Game::_debug_toggle_light() -> void {

	PRINT("_debug_toggle_light");

	state->set_lit(!state->get_lit());
};

auto Sorcery::Game::_debug_heal_party_to_full() -> void {

	PRINT("debug_heal_party_to_full");

	const auto party{state->get_party_characters()};
	for (auto idx : party) {
		auto &cur_char{characters.at(idx)};
		cur_char.set_status(Enums::Character::CStatus::OK);
		cur_char.set_current_hp(cur_char.get_max_hp());
		cur_char.reset_adjustment_per_turn();
		cur_char.set_poisoned_rate(0);
	}
}

auto Sorcery::Game::_debug_give_party_xp() -> void {

	PRINT("debug_give_party_xp");

	const auto party{state->get_party_characters()};
	for (auto idx : party) {
		auto &cur_char{characters.at(idx)};
		cur_char.grant_xp(cur_char.get_level() * 1000);
	}
}

auto Sorcery::Game::_debug_give_party_random_items() -> void {

	PRINT("debug_give_party_random_items");

	using Enums::Items::TypeID;

	const auto party{state->get_party_characters()};
	for (auto idx : party) {
		auto &cur_char{characters.at(idx)};
		auto slots_free = cur_char.inventory.get_empty_slots();
		for (auto i = 0u; i < slots_free; i++) {
			if (cur_char.inventory.get_empty_slots() > 0) {
				auto item{_resources->items->get_random_item(TypeID::LONG_SWORD,
					TypeID::RING_OF_DEATH)};
				cur_char.inventory.add(item);
			}
		}
	}
}

auto Sorcery::Game::_debug_give_party_gold() -> void {

	PRINT("debug_give_party_gold");

	const auto party{state->get_party_characters()};
	for (auto idx : party) {
		auto &cur_char{characters.at(idx)};
		cur_char.set_gold(1000);
	}
}

auto Sorcery::Game::_debug_create_random_party() -> void {

	using Enums::Character::Align;
	using Enums::Character::Class;
	using Enums::Items::TypeID;

	// Clear any existing party
	state->clear_party();

	PRINT("debug_create_random_party");

	// Create a new random party of a random alignment
	const auto align{(*_system->random)[Enums::System::Random::D2] == 1
						 ? Align::GOOD
						 : Align::EVIL};
	for (int i = 0; i < 6; i++) {
		auto pc{Character(_system, _resources)};
		switch (i) {
		case 0:
			pc.create_class_alignment(Class::FIGHTER, align);
			break;
		case 1:
			pc.create_class_alignment(Class::FIGHTER, Align::NEUTRAL);
			break;
		case 2:
			pc.create_class_alignment(Class::THIEF, Align::NEUTRAL);
			break;
		case 3:
			pc.create_class_alignment(Class::PRIEST, align);
			break;
		case 4:
			pc.create_class_alignment(Class::BISHOP, align);
			break;
		case 5:
			pc.create_class_alignment(Class::MAGE, Align::NEUTRAL);
			break;
		default:
			return;
		}

		pc.finalise();
		pc.set_location(Enums::Character::Location::PARTY);
		pc.set_stage(Enums::Character::Stage::COMPLETED);
		pc.inventory.clear();

		switch (pc.get_class()) { // NOLINT(clang-diagnostic-switch)
		case Class::FIGHTER:
		case Class::LORD:
		case Class::SAMURAI:
			pc.inventory.add_type((*_resources->items)[TypeID::LEATHER_ARMOR],
								  true);
			pc.inventory.add_type((*_resources->items)[TypeID::LONG_SWORD],
								  true);
			break;
		case Class::MAGE:
			pc.inventory.add_type((*_resources->items)[TypeID::ROBES], true);
			pc.inventory.add_type((*_resources->items)[TypeID::DAGGER], true);
			break;
		case Class::PRIEST:
		case Class::BISHOP:
			pc.inventory.add_type((*_resources->items)[TypeID::ROBES], true);
			pc.inventory.add_type((*_resources->items)[TypeID::STAFF], true);
			break;
		case Class::THIEF:
		case Class::NINJA:
			pc.inventory.add_type((*_resources->items)[TypeID::LEATHER_ARMOR],
								  true);
			pc.inventory.add_type((*_resources->items)[TypeID::SHORT_SWORD],
								  true);
		default:
			break;
		}

		auto char_id{save_character(pc)};
		characters[char_id] = pc;
		state->add_character_to_party(char_id);
	}

	save_game();
}

auto Sorcery::Game::_debug_fill_party_unid_items() -> void {

	PRINT("debug_fill_party_unid_items");

	const auto party{state->get_party_characters()};
	for (auto idx : party) {
		auto &cur_char{characters.at(idx)};
		auto slots_free = cur_char.inventory.get_empty_slots();
		for (auto i = 0u; i < slots_free; i++) {
			if (cur_char.inventory.get_empty_slots() > 0) {
				auto item{_resources->items->get_random_item(
					Enums::Items::TypeID::LONG_SWORD,
					Enums::Items::TypeID::RING_OF_DEATH)};
				cur_char.inventory.add(item);
			}
		}
	}

	save_game();
}

namespace Sorcery {
auto operator<<(std::ostream &out_stream, const Sorcery::Game &game)
	-> std::ostream & {

	auto text{"Game:\n\n"s};

	for (const auto &[char_id, character] : game.characters) {
		auto line{fmt::format("{}){:>16}{}", char_id, character.get_name(),
							  (int)character.get_location())};
		text.append(line);
		text.append("\n");
	}

	return out_stream << text << std::endl;
}

} // namespace Sorcery
