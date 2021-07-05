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

#include "main.hpp"

namespace Sorcery {

	class Database {

	  public:
		// Constructors
		Database(const std::filesystem::path &db_file_path);
		Database() = delete;

		// Public Methods
		auto has_game() -> bool;
		auto add_game() -> unsigned int;
		auto get_game() -> std::optional<GameEntry>;
		auto add_char(int game_id, std::string name, std::string data)
			-> unsigned int;
		auto get_chars(int game_id) -> std::vector<unsigned int>;
		auto get_char(int game_id, int character_id) -> std::string;
		auto delete_char(int game_id, int character_id) -> void;
		auto update_char(int game_id, int character_id, std::string name,
			std::string data) -> bool;

		// Public Members
		bool connected;

	  private:
		// Private Members
		std::filesystem::path _db_file_path;

		// Private Methods
	};
} // namespace Sorcery