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

#pragma once

#include "common/include.hpp"
#include "core/define.hpp"
#include "core/include.hpp"
#include "core/state.hpp"

#include "resources/itemstore.hpp"
#include "resources/levelstore.hpp"
#include "resources/monsterstore.hpp"
#include "types/character.hpp"

namespace Sorcery {

// Forward Declarations
class Display;
class Graphics;
class System;

// Game holds the current active game in progress
class Game {

	public:

		// Constructor
		Game(System *system, Display *display, Graphics *graphics);

		// Overloaded Operator
		auto friend operator<<(std::ostream &out_stream, const Game &game)
			-> std::ostream &;

		// Public Members
		bool valid;
		std::map<unsigned int, Character> characters;
		std::unique_ptr<State> state;
		std::unique_ptr<LevelStore> levelstore;
		std::unique_ptr<ItemStore> itemstore;
		std::unique_ptr<MonsterStore> monsterstore;

		// Public Methods
		auto wipe_data() -> void;
		auto get_id() const -> unsigned int;
		auto create_game() -> void;
		auto reset() -> void;
		auto load_game() -> void;
		auto save_game() -> void;
		auto add_character(Character character) -> unsigned int;
		auto delete_character(unsigned int char_id) -> void;
		auto update_character(
			unsigned game_id, unsigned char_id, Character &character) -> bool;
		auto enter_maze() -> void;
		auto show_console() -> void;
		auto hide_console() -> void;
		auto toggle_console() -> void;
		auto get_console_status() const -> bool;
		auto has_party_in_maze() const -> bool;
		auto get_characters_at_loc() const -> std::vector<unsigned int>;
		auto pass_turn(unsigned int turns = 1) -> void;
		auto get_turns() const -> unsigned int;
		auto divvy_party_gold() -> void;
		auto pool_party_gold(unsigned int char_id) -> void;
		auto log(const std::string &message, const int dice = -1,
			const int roll = -1, const int needed = -1) -> void;
		auto get_event(MAV event_type) const -> DungeonEvent;
		auto enable_event(MAV event_type) -> void;
		auto disable_event(MAV event_type) -> void;
		auto print() -> void;

		auto debug_create_random_party() -> void;
		auto debug_fill_party_unid_items() -> void;
		auto debug_give_party_gold() -> void;

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

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		TimePoint _start_time;
		TimePoint _last_time;
		std::string _key;
		unsigned int _id;
		std::string _status;
		std::vector<unsigned int> _char_ids;
		bool _show_console;
		std::vector<DungeonEvent> _events;
};

}
