// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not,
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
#include "system.hpp"

namespace Sorcery {

	// Game holds the current active game in progress
	class Game {

	  public:
		// Constructor
		Game(System *system, Display *display, Graphics *graphics);

		// Public Members
		bool valid;

		// Public Methods
		auto get_id() -> unsigned int;
		auto start_new_game() -> void;
		auto save_new_character(Character &character) -> unsigned int;
		auto load_characters() -> std::vector<Character>;

	  private:
		// Private Methods

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
		std::vector<Character> _characters;
	};
} // namespace Sorcery
