// Copyright (C) 2021 Dave Moore
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

#include "character.hpp"
#include "display.hpp"
#include "graphics.hpp"
#include "main.hpp"
#include "levelstore.hpp"
#include "state.hpp"
#include "system.hpp"

namespace Sorcery {

	// Game holds the current active game in progress
	class Game {

	  public:
		// Constructor
		Game(System *system, Display *display, Graphics *graphics);

		// Public Members
		bool valid;
		std::map<unsigned int, Character> characters;
		std::unique_ptr<State> state;
		std::unique_ptr<LevelStore> levelstore;

		// Public Methods
		auto get_id() -> unsigned int;
		auto create_game() -> void;
		auto load_game() -> void;
		auto save_game() -> void;
		auto add_character(Character &character) -> unsigned int;
		auto delete_character(unsigned int character_id) -> void;
		auto update_character(unsigned game_id, unsigned character_id,
			Character &character) -> bool;
		auto set_starting_level() -> void;

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

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		std::chrono::system_clock::time_point _start_time;
		std::chrono::system_clock::time_point _last_time;
		std::string _key;
		unsigned int _id;
		std::string _status;
		std::vector<unsigned int> _characters_ids;
	};
} // namespace Sorcery
