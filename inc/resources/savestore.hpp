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

#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Sorcery {

struct GameEntry;

class SaveStore {
	public:
		explicit SaveStore(const std::filesystem::path &game_file,
						   const std::filesystem::path &characters_directory);
		SaveStore() = delete;

		auto has_game() const -> bool;
		auto wipe_data() -> void;
		auto create_game_state(std::string key, std::string data)
			-> unsigned int;
		auto load_game_state() const -> std::optional<GameEntry>;
		auto save_game_state(unsigned int game_id, std::string_view key,
							 std::string data) -> void;
		auto add_character(unsigned int game_id, std::string name,
						   std::string data) -> unsigned int;
		auto update_character(unsigned int game_id, unsigned int character_id,
							  std::string name, std::string data) -> bool;
		auto delete_character(unsigned int game_id, unsigned int character_id)
			-> void;
		auto get_character_ids(unsigned int game_id) const
			-> std::vector<unsigned int>;
		auto get_character(unsigned int game_id,
						   unsigned int character_id) const -> std::string;

	private:
		std::filesystem::path _game_file;
		std::filesystem::path _characters_directory;

		auto _to_epoch_seconds(const std::chrono::system_clock::time_point time)
			const -> std::int64_t;
		auto _from_epoch_seconds(const std::int64_t seconds) const
			-> std::chrono::system_clock::time_point;
};

}