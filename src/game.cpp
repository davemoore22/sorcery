// Copyright (C) 2023 Dave Moore
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

#include "game.hpp"

// need to load level stores into this

Sorcery::Game::Game(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	valid = _system->database->has_game();
	if (valid) {
		_clear();
		_load_game();
	} else {
		_create_game();
		_load_game();
	}

	levelstore = std::make_unique<LevelStore>(_system, (*_system->files)[LEVELS_FILE]);

	_set_up_dungeon_events();
}

// Mappings for each type of special dungeon event in the Proving Grounds
auto Sorcery::Game::_set_up_dungeon_events() -> void {

	using enum Enums::Map::Event;

	_dungeon_events.clear();

	// bool params are:
	//
	// search after
	// encounter after
	// go back a square after
	// teleport back to town

	// Level 1
	_dungeon_events.emplace_back(AREA_OF_OUT_BOUNDS, "event_area_out_of_bounds", false, false, false, false);
	_dungeon_events.emplace_back(MAN_TELEPORT_CASTLE, "event_man_teleport_castle", false, false, false, true);
	_dungeon_events.emplace_back(SILVER_KEY, "event_silver_key", true, false, false, false);
	_dungeon_events.emplace_back(BRONZE_KEY, "event_bronze_key", true, false, false, false);
	_dungeon_events.emplace_back(MURPHYS_GHOSTS, "event_murphys_ghosts", true, true, false, false);

	// Level 2
	_dungeon_events.emplace_back(GOLD_KEY, "event_gold_key", true, false, false, false);
	_dungeon_events.emplace_back(BEAR_STATUE, "event_bear_statue", true, false, false, false);
	_dungeon_events.emplace_back(FROG_STATUE, "event_frog_statue", true, false, false, false);
	_dungeon_events.emplace_back(PLACARD_PIT_1, "event_placard_pit_1", false, false, false, false);
	_dungeon_events.emplace_back(PLACARD_PIT_2, "event_placard_pit_2", false, false, false, false);
	_dungeon_events.emplace_back(PLACARD_PIT_3, "event_placard_pit_3", false, false, false, false);
	_dungeon_events.emplace_back(NEED_SILVER_KEY, "event_need_silver_key", false, false, true, false);
	_dungeon_events.emplace_back(NEED_BRONZE_KEY, "event_need_bronze_key", false, false, true, false);
	_dungeon_events.emplace_back(NEED_BEAR_STATUE, "event_cannot_break_doors_down", false, false, true, false);
	_dungeon_events.emplace_back(NEED_FROG_STATUE, "event_cannot_break_doors_down", false, false, true, false);

	// Level 3
	_dungeon_events.emplace_back(TURN_LEFT, "event_turn_left", false, false, false, false);
	_dungeon_events.emplace_back(TURN_AROUND, "event_turn_around", false, false, false, false);
	_dungeon_events.emplace_back(TURN_RIGHT, "event_turn_right", false, false, false, false);

	// Level 4
	_dungeon_events.emplace_back(NEED_BEAR_STATUE_2, "event_need_bear_statue_2", false, false, true, false);
	_dungeon_events.emplace_back(TESTING_GROUNDS, "event_testing_grounds", false, false, false, false);
	_dungeon_events.emplace_back(ALARM_BELLS, "event_alarm_bells", false, true, false, false);
	_dungeon_events.emplace_back(TREASURE_REPOSITORY, "event_treasure_repository", false, false, false, false);
	_dungeon_events.emplace_back(
		MONSTER_ALLOCATION_CENTRE, "event_monster_allocation_centre", false, false, false, false);
	_dungeon_events.emplace_back(NEED_BLUE_RIBBON, "event_need_blue_ribbon", false, false, true, false);
	_dungeon_events.emplace_back(SERVICE_ELEVATOR, "event_service_elevator", false, false, false, false);
	_dungeon_events.emplace_back(GUARANTEED_COMBAT, "event_guaranteed_combat", false, true, false, false);
	_dungeon_events.emplace_back(LARGE_DESK, "event_large_desk", false, false, false, false);
	_dungeon_events.emplace_back(DEADLY_RING_COMBAT, "event_deadly_ring_combat", false, true, false, false);
	_dungeon_events.emplace_back(TREBOR_VOICE, "event_trebor_voice", false, false, false, false);

	// Level 6
	_dungeon_events.emplace_back(THREE_HUMANOIDS, "event_three_humanoids", false, false, false, false);

	// Level 7
	_dungeon_events.emplace_back(GETTING_WARM_1, "event_getting_warm_1", false, false, false, false);
	_dungeon_events.emplace_back(GETTING_WARM_2, "event_getting_warm_2", false, false, false, false);
	_dungeon_events.emplace_back(GETTING_WARM_3, "event_getting_warm_3", false, false, false, false);
	_dungeon_events.emplace_back(FIRE_DRAGONS_COMBAT, "event_fire_dragons_combat", false, true, false, false);

	// Level 10
	_dungeon_events.emplace_back(WERDNA_BOAST, "event_werdna_boast", false, false, false, false);
	_dungeon_events.emplace_back(TURN_BACK, "event_turn_back", false, false, false, false);
	_dungeon_events.emplace_back(WERDNA_SIGN, "event_werdna_sign_1", false, false, false, false);
}

auto Sorcery::Game::get_event(MapEvent event_type) const -> DungeonEvent {

	auto it{std::ranges::find_if(_dungeon_events.begin(), _dungeon_events.end(),
		[&](const auto &dungeon_event) { return (dungeon_event.event == event_type); })};

	return *it;

	// TODO: handle updating of these
}

auto Sorcery::Game::enable_event(MapEvent event_type) -> void {

	auto it{std::ranges::find_if(_dungeon_events.begin(), _dungeon_events.end(),
		[&](const auto &dungeon_event) { return (dungeon_event.event == event_type); })};

	if (it != _dungeon_events.end())
		(*it).enabled = true;
}

auto Sorcery::Game::disable_event(MapEvent event_type) -> void {

	auto it{std::ranges::find_if(_dungeon_events.begin(), _dungeon_events.end(),
		[&](const auto &dungeon_event) { return (dungeon_event.event == event_type); })};

	if (it != _dungeon_events.end())
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

auto Sorcery::Game::delete_character(unsigned int character_id) -> void {

	_system->database->delete_character(_id, character_id);
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
	_characters_ids.clear();
	state = std::make_unique<State>(_system);
	levelstore = std::make_unique<LevelStore>(_system, (*_system->files)[LEVELS_FILE]);

	state->clear_log_messages();

	state->level->reset();
}

auto Sorcery::Game::_create_game() -> void {

	_clear();
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		state->add_log_message("New Game Started", MessageType::GAME);
		archive(state);
	}
	const auto data{ss.str()};
	_id = _system->database->create_game_state(data);
}

auto Sorcery::Game::_load_game() -> void {

	// Get Game and State Data
	auto [id, key, status, start_time, last_time, data] = _system->database->load_game_state().value();
	_id = id;
	_key = key;
	_status = status;
	_start_time = start_time;
	_last_time = last_time;
	state = std::make_unique<State>(_system);
	levelstore = std::make_unique<LevelStore>(_system, (*_system->files)[LEVELS_FILE]);
	if (data.length() > 0) {
		std::stringstream ss;
		ss.str(data);
		{
			cereal::JSONInputArchive archive(ss);
			archive(state);
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
		cereal::JSONOutputArchive archive(ss);
		archive(state);
	}
	const auto data{ss.str()};

	_system->database->save_game_state(_id, _key, data);
	_save_characters();
}

auto Sorcery::Game::_save_characters() -> void {
	for (const auto &[character_id, character] : characters) {

		_update_party_location();
		std::stringstream ss;
		{
			cereal::JSONOutputArchive archive(ss);
			archive(character);
		}
		const auto character_data{ss.str()};

		_system->database->update_character(_id, character_id, character.get_name(), character_data);
	}
}

auto Sorcery::Game::has_party_in_maze() const -> bool {

	for (auto &[character_id, character] : characters) {

		if (character.get_location() == CharacterLocation::MAZE)
			return true;
	}

	return false;
}

auto Sorcery::Game::_update_party_location() -> void {

	auto party{state->get_party_characters()};
	for (auto &[character_id, character] : characters) {
		if (std::find(party.begin(), party.end(), character_id) != party.end()) {
			character.depth = state->get_depth();
			character.coordinate = state->get_player_pos();
		}
	}
}

auto Sorcery::Game::get_characters_at_loc() const -> std::vector<unsigned int> {

	std::vector<unsigned int> results;
	for (auto &[character_id, character] : characters) {
		if (character.get_location() == CharacterLocation::MAZE) {
			if (character.coordinate == state->get_player_pos()) {
				results.emplace_back(character_id);
			}
		}
	}

	return results;
}

auto Sorcery::Game::_get_characters() -> std::map<unsigned int, Character> {

	std::map<unsigned int, Character> characters;
	characters.clear();

	for (auto character_id : _characters_ids) {

		const auto data{_system->database->get_character(_id, character_id)};
		std::stringstream ss;
		ss.str(data);

		// Remember that the three pointers aren't serialised
		Character character(_system, _display, _graphics);
		character.create_spells();
		{
			cereal::JSONInputArchive archive(ss);
			archive(character);
		}
		character.set_spells();
		character.set_stage(CharacterStage::COMPLETED);
		characters[character_id] = character;
	}

	return characters;
}

auto Sorcery::Game::add_character(Character &character) -> unsigned int {

	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		archive(character);
	}
	const auto character_data{ss.str()};

	return _system->database->add_character(_id, character.get_name(), character_data);
}

auto Sorcery::Game::update_character(unsigned int game_id, unsigned int character_id, Character &character) -> bool {
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		archive(character);
	}
	const auto character_data{ss.str()};

	return _system->database->update_character(game_id, character_id, character.get_name(), character_data);
}

auto Sorcery::Game::_load_characters() -> void {

	_characters_ids.clear();
	_characters_ids = _system->database->get_character_ids(_id);
	characters.clear();
	characters = _get_characters();
}

auto Sorcery::Game::divvy_party_gold() -> void {

	const auto party{state->get_party_characters()};
	auto gold{0};

	for (auto idx : party) {
		auto _cur_char{&characters.at(idx)};
		gold += _cur_char->get_gold();
	}

	gold = gold / party.size();
	for (auto idx : party) {
		auto _cur_char{&characters.at(idx)};
		_cur_char->set_gold(gold);
	}
}

auto Sorcery::Game::pool_party_gold(unsigned int char_id) -> void {

	const auto party{state->get_party_characters()};
	auto gold{0};

	for (auto idx : party) {
		auto _cur_char{&characters.at(idx)};
		gold += _cur_char->get_gold();
	}

	for (auto idx : party) {
		auto _cur_char{&characters.at(idx)};
		if (char_id != idx)
			_cur_char->set_gold(0);
		else
			_cur_char->set_gold(gold);
	}
}
