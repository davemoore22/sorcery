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

#include "game/game.hpp"
#include "cereal/archives/xml.hpp" // for epilogue, prologue, XMLOutp...
#include "common/enum.hpp"		   // for Event, Location, Align, Ali...
#include "common/macro.hpp"		   // for GUID
#include "core/context.hpp"		   // for Context
#include "core/debug.hpp"		   // for DEBUG_LOG
#include "core/resources.hpp"	   // for Resources
#include "game/cheat.hpp"		   // for Cheat
#include "game/spellcasting.hpp"   // for SpellCasting
#include "magic/castcontext.hpp"
#include "resources/itemstore.hpp"		 // for ItemStore
#include "resources/levelstore.hpp"		 // for LevelStore
#include "resources/savestore.hpp"		 // for SaveStore
#include "types/character/inventory.hpp" // for Inventory
#include "types/character/magic.hpp"	 // for CharacterMagic
#include "types/meta.hpp"				 // for enum_name
#include "types/scopedtimer.hpp"		 // for PROFILE_SCOPE
#include "types/state.hpp"				 // for State
#include "types/world/level.hpp"		 // for Level
#include <algorithm>					 // for find_if, find
#include <format>						 // for format
#include <optional>						 // for optional, operator==
#include <print>						 // for println
#include <ranges>						 // for __find_if_fn
#include <sstream>						 // for basic_stringstream, basic_o...
#include <string_view>					 // for basic_string_view
#include <utility>						 // for get

Sorcery::Game::Game(Context &ctx)
	: _ctx{ctx} {

	if (_ctx.saves->has_game()) {
		_clear();
		_load_game();
	} else {
		_create_game();
		_load_game();
	}

	_set_up_dungeon_events();

	_cheat = std::make_unique<Cheat>(_ctx, *this);
	_spellcasting = std::make_unique<SpellCasting>(_ctx, *this);
}

Sorcery::Game::~Game() = default;

auto Sorcery::Game::post_construct(Context &ctx) -> void {

	_ctx = ctx;
}

auto Sorcery::Game::reset() -> void {

	_create_game();
	_load_game();
}

auto Sorcery::Game::wipe_data() -> void {

	_ctx.saves->wipe_data();
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

	// TODO: hate this!

	// bool params are:
	//
	// search after
	// encounter after
	// go back a square after
	// teleport back to town
	// give item after

	// Level 1
	using enum Enums::Map::Event;
	_events.emplace_back(AREA_OF_OUT_BOUNDS, "event_area_out_of_bounds", false, false, false, false, false);
	_events.emplace_back(MAN_TELEPORT_CASTLE, "event_man_teleport_castle", false, false, false, true, false);
	_events.emplace_back(OBTAIN_SILVER_KEY, "event_silver_key", true, false, false, false, false);
	_events.emplace_back(OBTAIN_BRONZE_KEY, "event_bronze_key", true, false, false, false, false);
	_events.emplace_back(MURPHYS_GHOSTS, "event_murphys_ghosts", true, true, false, false, false);

	_events.emplace_back(TOP_ELEVATOR, "event_top_elevator", false, false, false, false, false);

	// Level 2
	_events.emplace_back(OBTAIN_GOLD_KEY, "event_gold_key", true, false, false, false, false);
	_events.emplace_back(OBTAIN_BEAR_STATUE, "event_bear_statue", true, false, false, false, false);
	_events.emplace_back(OBTAIN_FROG_STATUE, "event_frog_statue", true, false, false, false, false);
	_events.emplace_back(PLACARD_PIT_1, "event_placard_pit_1", false, false, false, false, false);
	_events.emplace_back(PLACARD_PIT_2, "event_placard_pit_2", false, false, false, false, false);
	_events.emplace_back(PLACARD_PIT_3, "event_placard_pit_3", false, false, false, false, false);
	_events.emplace_back(NEED_SILVER_KEY, "event_need_silver_key", false, false, true, false, false);
	_events.emplace_back(NEED_BRONZE_KEY, "event_need_bronze_key", false, false, true, false, false);
	_events.emplace_back(NEED_BEAR_STATUE, "event_cannot_break_doors_down", false, false, true, false, false);
	_events.emplace_back(NEED_FROG_STATUE, "event_cannot_break_doors_down", false, false, true, false, false);

	// Level 3
	_events.emplace_back(TURN_LEFT, "event_turn_left", false, false, false, false, false);
	_events.emplace_back(TURN_AROUND, "event_turn_around", false, false, false, false, false);
	_events.emplace_back(TURN_RIGHT, "event_turn_right", false, false, false, false, false);

	// Level 4
	_events.emplace_back(NEED_GOLD_KEY, "event_need_gold_key", false, false, true, false, false);
	_events.emplace_back(TESTING_GROUNDS, "event_testing_grounds", false, false, false, false, false);
	_events.emplace_back(ALARM_BELLS, "event_alarm_bells", false, true, false, false, false);
	_events.emplace_back(TREASURE_REPOSITORY, "event_treasure_repository", false, false, false, false, false);
	_events.emplace_back(MONSTER_ALLOCATION_CENTRE, "event_monster_allocation_centre", false, false, false, false,
						 false);
	_events.emplace_back(NEED_BLUE_RIBBON, "event_need_blue_ribbon", false, false, true, false, false);
	_events.emplace_back(SERVICE_ELEVATOR, "event_service_elevator", false, false, false, false, false);
	_events.emplace_back(LARGE_DESK, "event_large_desk", false, false, false, false, false);
	_events.emplace_back(DEADLY_RING_COMBAT, "event_deadly_ring_combat", false, true, false, false, false);
	_events.emplace_back(TREBOR_VOICE, "event_trebor_voice", false, false, false, false, true);

	// Level 6
	_events.emplace_back(THREE_HUMANOIDS, "event_three_humanoids", false, false, false, false, false);

	// Level 7
	_events.emplace_back(GETTING_WARM_1, "event_getting_warm_1", false, false, false, false, false);
	_events.emplace_back(GETTING_WARM_2, "event_getting_warm_2", false, false, false, false, false);
	_events.emplace_back(GETTING_WARM_3, "event_getting_warm_3", false, false, false, false, false);
	_events.emplace_back(FIRE_DRAGONS_COMBAT, "event_fire_dragons_combat", false, true, false, false, false);

	// Level 10
	_events.emplace_back(WERDNA_BOAST, "event_werdna_boast", false, false, false, false, false);
	_events.emplace_back(TURN_BACK, "event_turn_back", false, false, false, false, false);
	_events.emplace_back(WERDNA_SIGN_IN, "event_werdna_sign_1", false, false, false, false, false);
	_events.emplace_back(WERDNA_SIGN_OUT, "event_werdna_sign_2", false, false, false, false, false);
}

auto Sorcery::Game::get_event(Enums::Map::Event event_type) const -> DungeonEvent {

	auto it{std::ranges::find_if(_events.begin(), _events.end(), [&](const auto &dungeon_event) {
		return (dungeon_event.event == event_type);
	})};

	return *it;

	// TODO: handle updating of these
}

auto Sorcery::Game::enable_event(Enums::Map::Event event_type) -> void {

	auto it{std::ranges::find_if(_events.begin(), _events.end(), [&](const auto &dungeon_event) {
		return (dungeon_event.event == event_type);
	})};

	if (it != _events.end())
		(*it).enabled = true;
}

auto Sorcery::Game::disable_event(Enums::Map::Event event_type) -> void {

	auto it{std::ranges::find_if(_events.begin(), _events.end(), [&](const auto &dungeon_event) {
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

	PROFILE_SCOPE("Game::load_game");
	DEBUG_LOG("GAME Loading Game from DB");

	_load_game();
}

auto Sorcery::Game::save_game() -> void {

	PROFILE_SCOPE("Game::save_game");
	DEBUG_LOG("GAME Saving Game to DB");

	_save_game();
}

auto Sorcery::Game::enter_maze() -> void {

	Level level{_ctx.resources->levels->get(-1).value()};

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
			if (character.depth.value() == to_depth && character.coordinate.value() == to_loc) {
				character.set_location(Enums::Character::Location::PARTY);
				state->add_character_by_id(id);
			}
		}
	}

	state->set_depth(to_depth);
	state->set_player_prev_depth(state->get_depth());
	state->set_player_pos(to_loc);
	Level level{_ctx.resources->levels->get(to_depth).value()};
	state->set_current_level(&level);
}

auto Sorcery::Game::delete_character(unsigned int char_id) -> void {

	if (!characters.contains(char_id))
		return;

	characters.erase(char_id);

	save_game();

	_ctx.saves->delete_character(_id, char_id);
}

auto Sorcery::Game::_clear() -> void {

	// Clear existing data!
	state.reset();
	characters.clear();
	_char_ids.clear();

	state = std::make_unique<State>(&_ctx);

	state->clear_log_messages();
	state->reset_shop(_ctx.resources->items.get());

	state->level->reset();
}

auto Sorcery::Game::_create_game() -> void {
	_clear();

	std::stringstream ss;
	{
		cereal::XMLOutputArchive out_archive(ss);
		state->add_log_message("New Game Started", Enums::Internal::MessageType::GAME);
		out_archive(state);
	}
	const auto data{ss.str()};

	_key = GUID();
	_id = _ctx.saves->create_game_state(_key, data);
}

auto Sorcery::Game::_load_game() -> void {

	// Get Game and State Data
	auto [id, key, status, start_time, last_time, data] = _ctx.saves->load_game_state().value();
	_id = id;
	_key = key;
	_status = status;
	_start_time = start_time;
	_last_time = last_time;
	state = std::make_unique<State>();
	if (data.length() > 0) {
		std::stringstream ss;
		ss.str(data);
		{
			cereal::XMLInputArchive in_archive(ss);
			in_archive(state);
			state->set(&_ctx);
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

	_ctx.saves->save_game_state(_id, _key, data);
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

		_ctx.saves->update_character(_id, char_id, character.get_name(), char_data);
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

	return _ctx.saves->add_character(_id, character.get_name(), char_data);
}

auto Sorcery::Game::update_character(unsigned int game_id, unsigned int char_id, Character &character) -> bool {

	std::stringstream ss;
	{
		cereal::XMLOutputArchive out_archive(ss);
		out_archive(character);
	}
	const auto character_data{ss.str()};

	return _ctx.saves->update_character(game_id, char_id, character.get_name(), character_data);
}

auto Sorcery::Game::_load_characters() -> void {

	_char_ids.clear();
	_char_ids = _ctx.saves->get_character_ids(_id);
	characters.clear();

	for (auto char_id : _char_ids) {

		const auto data{_ctx.saves->get_character(_id, char_id)};
		std::stringstream ss;
		ss.str(data);

		Character character(&_ctx);
		{
			cereal::XMLInputArchive in_archive(ss);
			in_archive(character);
		}
		character.magic().create_spells();
		character.magic().set_spells();
		characters[char_id] = character;
	}
}

auto Sorcery::Game::get_party_alignment() const -> Enums::Character::Align {

	auto good{0u}, evil{0u};
	using enum Enums::Character::Align;
	for (const auto party{state->get_party_characters()}; auto idx : party) {
		auto cur_char{&characters.at(idx)};
		if (cur_char->get_alignment() == GOOD)
			++good;
		else if (cur_char->get_alignment() == EVIL)
			++evil;
	}

	if (good > 0)
		return GOOD;
	else if (evil > 0)
		return EVIL;
	else
		return NEUTRAL;
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

	using namespace std::string_literals;
	auto text{"Game:\n\n"s};

	for (const auto &[char_id, character] : characters) {
		auto line{std::format("{}){:>16} {}", char_id, character.get_name(), enum_name(character.get_location()))};
		text.append(line);
		text.append("\n");
	}

	text.append("\n[");
	for (const auto char_id : state->get_party_characters()) {
		auto line{std::format("{},", char_id)};
		text.append(line);
	}
	text.append("]\n");
	std::println("{}", text);
}

auto Sorcery::Game::give_party_item(const Enums::Items::TypeID item_type, const bool known) -> bool {

	for (const auto char_id : _ctx.game->state->get_party_characters()) {

		auto &character{_ctx.game->characters.at(char_id)};
		if (character.inventory.get_empty_slots() == 0)
			continue;
		character.inventory.add_type(_ctx.resources->items->get(item_type), known);
		return true;
	}

	return false;
}

auto Sorcery::Game::party_has_item(const Enums::Items::TypeID item_type) const -> bool {

	for (const auto party{state->get_party_characters()}; auto idx : party) {
		const auto &cur_char{characters.at(idx)};
		if (cur_char.inventory.has_item(item_type))
			return true;
	}

	return false;
}

auto Sorcery::Game::remove_party_item(const Enums::Items::TypeID item_type) -> bool {

	for (const auto char_id : state->get_party_characters()) {

		auto &character{characters.at(char_id)};

		if (character.inventory.discard_item(item_type))
			return true;
	}

	return false;
}

auto Sorcery::Game::set_party_members_wiz_1_award(const bool value) -> void {

	for (const auto char_id : state->get_party_characters())
		characters.at(char_id).set_wiz_1_awatd(value);
}

auto Sorcery::Game::grant_party_members_gold(const int value) -> void {

	for (const auto char_id : state->get_party_characters())
		characters.at(char_id).grant_gold(value);
}

auto Sorcery::Game::grant_party_members_xp(const int adjustment) -> void {

	for (const auto char_id : state->get_party_characters())
		characters.at(char_id).grant_xp(adjustment);
}

auto Sorcery::Game::cheat() -> Cheat & {

	return *_cheat;
}

auto Sorcery::Game::spellcasting() -> SpellCasting & {

	return *_spellcasting;
}

namespace Sorcery {
auto operator<<(std::ostream &out_stream, const Sorcery::Game &game) -> std::ostream & {

	using namespace std::string_literals;
	auto text{"Game:\n\n"s};

	for (const auto &[char_id, character] : game.characters) {
		auto line{std::format("{}){:>16}{}", char_id, character.get_name(), (int)character.get_location())};
		text.append(line);
		text.append("\n");
	}

	return out_stream << text << std::endl;
}

} // namespace Sorcery
