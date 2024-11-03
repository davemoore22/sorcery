// Copyright (C) 2024 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Shadows under Llylgamyn.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "core/game.hpp"
#include "core/display.hpp"
#include "core/graphics.hpp"
#include "core/system.hpp"

// need to load level stores into this

Sorcery::Game::Game(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	itemstore =
		std::make_unique<ItemStore>(_system, (*_system->files)[ITEMS_FILE]);
	monsterstore = std::make_unique<MonsterStore>(
		_system, (*_system->files)[MONSTERS_FILE]);

	valid = _system->database->has_game();
	if (valid) {
		_clear();
		_load_game();
	} else {
		_create_game();
		_load_game();
	}

	levelstore =
		std::make_unique<LevelStore>(_system, (*_system->files)[LEVELS_FILE]);

	_set_up_dungeon_events();
}

auto Sorcery::Game::reset() -> void {

	valid = false;
	_create_game();
	_load_game();
}

auto Sorcery::Game::wipe_data() -> void {

	_system->database->wipe_data();
}

// Mappings for each type of special dungeon event in the Proving Grounds
auto Sorcery::Game::_set_up_dungeon_events() -> void {

	_events.clear();

	// bool params are:
	//
	// search after
	// encounter after
	// go back a square after
	// teleport back to town

	// Level 1
	_events.emplace_back(MAV::AREA_OF_OUT_BOUNDS, "event_area_out_of_bounds",
		false, false, false, false);
	_events.emplace_back(MAV::MAN_TELEPORT_CASTLE, "event_man_teleport_castle",
		false, false, false, true);
	_events.emplace_back(
		MAV::SILVER_KEY, "event_silver_key", true, false, false, false);
	_events.emplace_back(
		MAV::BRONZE_KEY, "event_bronze_key", true, false, false, false);
	_events.emplace_back(
		MAV::MURPHYS_GHOSTS, "event_murphys_ghosts", true, true, false, false);

	// Level 2
	_events.emplace_back(
		MAV::GOLD_KEY, "event_gold_key", true, false, false, false);
	_events.emplace_back(
		MAV::BEAR_STATUE, "event_bear_statue", true, false, false, false);
	_events.emplace_back(
		MAV::FROG_STATUE, "event_frog_statue", true, false, false, false);
	_events.emplace_back(
		MAV::PLACARD_PIT_1, "event_placard_pit_1", false, false, false, false);
	_events.emplace_back(
		MAV::PLACARD_PIT_2, "event_placard_pit_2", false, false, false, false);
	_events.emplace_back(
		MAV::PLACARD_PIT_3, "event_placard_pit_3", false, false, false, false);
	_events.emplace_back(MAV::NEED_SILVER_KEY, "event_need_silver_key", false,
		false, true, false);
	_events.emplace_back(MAV::NEED_BRONZE_KEY, "event_need_bronze_key", false,
		false, true, false);
	_events.emplace_back(MAV::NEED_BEAR_STATUE, "event_cannot_break_doors_down",
		false, false, true, false);
	_events.emplace_back(MAV::NEED_FROG_STATUE, "event_cannot_break_doors_down",
		false, false, true, false);

	// Level 3
	_events.emplace_back(
		MAV::TURN_LEFT, "event_turn_left", false, false, false, false);
	_events.emplace_back(
		MAV::TURN_AROUND, "event_turn_around", false, false, false, false);
	_events.emplace_back(
		MAV::TURN_RIGHT, "event_turn_right", false, false, false, false);

	// Level 4
	_events.emplace_back(MAV::NEED_BEAR_STATUE_2, "event_need_bear_statue_2",
		false, false, true, false);
	_events.emplace_back(MAV::TESTING_GROUNDS, "event_testing_grounds", false,
		false, false, false);
	_events.emplace_back(
		MAV::ALARM_BELLS, "event_alarm_bells", false, true, false, false);
	_events.emplace_back(MAV::TREASURE_REPOSITORY, "event_treasure_repository",
		false, false, false, false);
	_events.emplace_back(MAV::MONSTER_ALLOCATION_CENTRE,
		"event_monster_allocation_centre", false, false, false, false);
	_events.emplace_back(MAV::NEED_BLUE_RIBBON, "event_need_blue_ribbon", false,
		false, true, false);
	_events.emplace_back(MAV::SERVICE_ELEVATOR, "event_service_elevator", false,
		false, false, false);
	_events.emplace_back(MAV::GUARANTEED_COMBAT, "event_guaranteed_combat",
		false, true, false, false);
	_events.emplace_back(
		MAV::LARGE_DESK, "event_large_desk", false, false, false, false);
	_events.emplace_back(MAV::DEADLY_RING_COMBAT, "event_deadly_ring_combat",
		false, true, false, false);
	_events.emplace_back(
		MAV::TREBOR_VOICE, "event_trebor_voice", false, false, false, false);

	// Level 6
	_events.emplace_back(MAV::THREE_HUMANOIDS, "event_three_humanoids", false,
		false, false, false);

	// Level 7
	_events.emplace_back(MAV::GETTING_WARM_1, "event_getting_warm_1", false,
		false, false, false);
	_events.emplace_back(MAV::GETTING_WARM_2, "event_getting_warm_2", false,
		false, false, false);
	_events.emplace_back(MAV::GETTING_WARM_3, "event_getting_warm_3", false,
		false, false, false);
	_events.emplace_back(MAV::FIRE_DRAGONS_COMBAT, "event_fire_dragons_combat",
		false, true, false, false);

	// Level 10
	_events.emplace_back(
		MAV::WERDNA_BOAST, "event_werdna_boast", false, false, false, false);
	_events.emplace_back(
		MAV::TURN_BACK, "event_turn_back", false, false, false, false);
	_events.emplace_back(
		MAV::WERDNA_SIGN, "event_werdna_sign_1", false, false, false, false);
}

auto Sorcery::Game::get_event(MAV event_type) const -> DungeonEvent {

	auto it{std::ranges::find_if(
		_events.begin(), _events.end(), [&](const auto &dungeon_event) {
			return (dungeon_event.event == event_type);
		})};

	return *it;

	// TODO: handle updating of these
}

auto Sorcery::Game::enable_event(MAV event_type) -> void {

	auto it{std::ranges::find_if(
		_events.begin(), _events.end(), [&](const auto &dungeon_event) {
			return (dungeon_event.event == event_type);
		})};

	if (it != _events.end())
		(*it).enabled = true;
}

auto Sorcery::Game::disable_event(MAV event_type) -> void {

	auto it{std::ranges::find_if(
		_events.begin(), _events.end(), [&](const auto &dungeon_event) {
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

	Level level{((*levelstore)[-1]).value()};
	state->set_current_level(&level);
	state->restart_expedition();
}

auto Sorcery::Game::delete_character(unsigned int char_id) -> void {

	_system->database->delete_character(_id, char_id);
}

auto Sorcery::Game::_clear() -> void {

	// Clear existing data!
	if (state.get()) {
		state.release();
		state.reset();
	}
	if (levelstore.get()) {
		levelstore.release();
		levelstore.reset();
	}

	characters.clear();
	_char_ids.clear();
	state = std::make_unique<State>(_system);
	levelstore =
		std::make_unique<LevelStore>(_system, (*_system->files)[LEVELS_FILE]);

	state->clear_log_messages();
	state->reset_shop(itemstore.get());

	state->level->reset();
}

auto Sorcery::Game::_create_game() -> void {

	_clear();
	std::stringstream ss;
	{
		cereal::XMLOutputArchive out_archive(ss);
		state->add_log_message("New Game Started", IMT::GAME);
		out_archive(state);
	}
	const auto data{ss.str()};
	_id = _system->database->create_game_state(data);
}

auto Sorcery::Game::_load_game() -> void {

	// Get Game and State Data
	auto [id, key, status, start_time, last_time, data] =
		_system->database->load_game_state().value();
	_id = id;
	_key = key;
	_status = status;
	_start_time = start_time;
	_last_time = last_time;
	state = std::make_unique<State>();
	levelstore =
		std::make_unique<LevelStore>(_system, (*_system->files)[LEVELS_FILE]);
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

	_system->database->save_game_state(_id, _key, data);
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

		_system->database->update_character(
			_id, char_id, character.get_name(), char_data);
	}
}

auto Sorcery::Game::has_party_in_maze() const -> bool {

	for (auto &[char_id, character] : characters) {

		if (character.get_location() == CHL::MAZE)
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
		if (character.get_location() == CHL::MAZE) {
			if (character.coordinate == state->get_player_pos()) {
				results.emplace_back(char_id);
			}
		}
	}

	return results;
}

auto Sorcery::Game::add_character(Character character) -> unsigned int {

	std::stringstream ss;
	{
		cereal::XMLOutputArchive out_archive(ss);
		out_archive(character);
	}
	const auto char_data{ss.str()};

	return _system->database->add_character(
		_id, character.get_name(), char_data);
}

auto Sorcery::Game::update_character(
	unsigned int game_id, unsigned int char_id, Character &character) -> bool {

	std::stringstream ss;
	{
		cereal::XMLOutputArchive out_archive(ss);
		out_archive(character);
	}
	const auto character_data{ss.str()};

	return _system->database->update_character(
		game_id, char_id, character.get_name(), character_data);
}

auto Sorcery::Game::_load_characters() -> void {

	_char_ids.clear();
	_char_ids = _system->database->get_character_ids(_id);
	characters.clear();

	for (auto char_id : _char_ids) {

		const auto data{_system->database->get_character(_id, char_id)};
		std::stringstream ss;
		ss.str(data);

		// Remember that the three pointers aren't serialised
		Character character(_system, _display, _graphics, itemstore.get());
		// Character character;
		{
			cereal::XMLInputArchive in_archive(ss);
			in_archive(character);
		}
		character.create_spells();
		character.set_spells();
		characters[char_id] = character;
	}
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

auto Sorcery::Game::debug_create_random_party() -> void {

	// Clear any existing party
	state->clear_party();

	PRINT("debug_create_random_party");

	// Create a new random party of a random alignment
	const auto align{(*_system->random)[RNT::D2] == 1 ? CAL::GOOD : CAL::EVIL};
	for (int i = 0; i < 6; i++) {
		auto pc{Character(_system, _display, _graphics, itemstore.get())};
		switch (i) {
		case 0:
			pc.create_class_alignment(CHC::FIGHTER, align);
			break;
		case 1:
			pc.create_class_alignment(CHC::FIGHTER, CAL::NEUTRAL);
			break;
		case 2:
			pc.create_class_alignment(CHC::THIEF, CAL::NEUTRAL);
			break;
		case 3:
			pc.create_class_alignment(CHC::PRIEST, align);
			break;
		case 4:
			pc.create_class_alignment(CHC::BISHOP, align);
			break;
		case 5:
			pc.create_class_alignment(CHC::MAGE, CAL::NEUTRAL);
			break;
		default:
			return;
		}

		pc.finalise();
		pc.set_location(CHL::PARTY);
		pc.set_stage(CHS::COMPLETED);
		pc.inventory.clear();

		switch (pc.get_class()) { // NOLINT(clang-diagnostic-switch)
		case CHC::FIGHTER:
		case CHC::LORD:
		case CHC::SAMURAI:
			pc.inventory.add_type((*itemstore)[ITT::LEATHER_ARMOR], true);
			pc.inventory.add_type((*itemstore)[ITT::LONG_SWORD], true);
			break;
		case CHC::MAGE:
			pc.inventory.add_type((*itemstore)[ITT::ROBES], true);
			pc.inventory.add_type((*itemstore)[ITT::DAGGER], true);
			break;
		case CHC::PRIEST:
		case CHC::BISHOP:
			pc.inventory.add_type((*itemstore)[ITT::ROBES], true);
			pc.inventory.add_type((*itemstore)[ITT::STAFF], true);
			break;
		case CHC::THIEF:
		case CHC::NINJA:
			pc.inventory.add_type((*itemstore)[ITT::LEATHER_ARMOR], true);
			pc.inventory.add_type((*itemstore)[ITT::SHORT_SWORD], true);
		default:
			break;
		}

		auto char_id{add_character(pc)};
		characters[char_id] = pc;
		state->add_character_by_id(char_id);
	}

	save_game();
}

auto Sorcery::Game::debug_fill_party_unid_items() -> void {

	PRINT("debug_fill_party_unid_items");

	const auto party{state->get_party_characters()};
	for (auto idx : party) {
		auto &cur_char{characters.at(idx)};
		auto slots_free = cur_char.inventory.get_empty_slots();
		for (auto i = 0u; i < slots_free; i++) {
			if (cur_char.inventory.get_empty_slots() > 0) {
				auto item{itemstore->get_random_item(
					ITT::LONG_SWORD, ITT::RING_OF_DEATH)};
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
