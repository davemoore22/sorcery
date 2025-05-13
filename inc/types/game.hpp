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

#pragma once

#include "types/character.hpp"

namespace Sorcery {

// Forward Declarations
class LevelStore;
class System;
class State;
class Resources;

// Game holds the current active game in progress
class Game {

	public:
		// Constructor
		Game(System *system, Resources *resources);

		// Overloaded Operator
		auto friend operator<<(std::ostream &out_stream, const Game &game)
			-> std::ostream &;

		// Public Members
		std::map<unsigned int, Character> characters;
		std::unique_ptr<State> state;
		std::unique_ptr<LevelStore> levels;

		// Public Methods
		auto wipe_data() -> void;
		auto get_id() const -> unsigned int;
		auto create_game() -> void;
		auto reset() -> void;
		auto load_game() -> void;
		auto save_game() -> void;
		auto save_character(Character character) -> unsigned int;
		auto delete_character(unsigned int char_id) -> void;
		auto update_character(unsigned game_id, unsigned char_id,
							  Character &character) -> bool;
		auto enter_maze() -> void;
		auto restart_maze(unsigned int char_id) -> void;
		auto show_console() -> void;
		auto hide_console() -> void;
		auto toggle_console() -> void;
		auto get_console_status() const -> bool;
		auto has_party_in_maze() const -> bool;
		auto get_party_alignment() const -> Enums::Character::Align;
		auto get_characters_at_loc() const -> std::vector<unsigned int>;
		auto pass_turn(unsigned int turns = 1) -> void;
		auto get_turns() const -> unsigned int;
		auto divvy_party_gold() -> void;
		auto move_party_to_tavern() -> void;
		auto pool_party_gold(unsigned int char_id) -> void;
		auto log(const std::string &message, const int dice = -1,
				 const int roll = -1, const int needed = -1) -> void;
		auto get_event(Enums::Map::Event event_type) const -> DungeonEvent;
		auto enable_event(Enums::Map::Event event_type) -> void;
		auto disable_event(Enums::Map::Event event_type) -> void;
		auto print() -> void;
		auto call_debug(const SDL_Keycode key_code) -> void;

	private:
		// Private Methods
		auto _clear() -> void;
		auto _create_game() -> void;
		auto _save_game() -> void;
		auto _load_game() -> void;
		auto _load_characters() -> void;
		auto _load_levels() -> void;
		auto _get_characters() -> std::map<unsigned int, Character>;
		auto _save_characters() -> void;
		auto _update_party_location() -> void;
		auto _set_up_dungeon_events() -> void;
		auto _set_up_debug_keys() -> void;

		auto _debug_create_random_party() -> void;
		auto _debug_fill_party_unid_items() -> void;
		auto _debug_give_party_gold() -> void;
		auto _debug_give_party_random_items() -> void;
		auto _debug_give_party_random_status() -> void;
		auto _debug_give_party_xp() -> void;
		auto _debug_heal_party_to_full() -> void;
		auto _debug_harm_party_to_min() -> void;
		auto _debug_toggle_light() -> void;

		// Private Members
		System *_system;
		Resources *_resources;
		std::chrono::time_point<std::chrono::system_clock> _start_time;
		std::chrono::time_point<std::chrono::system_clock> _last_time;
		std::string _key;
		unsigned int _id;
		std::string _status;
		std::vector<unsigned int> _char_ids;
		bool _show_console;
		std::vector<DungeonEvent> _events;
		std::map<SDL_Keycode, std::function<void()>> _debug;
};

}
