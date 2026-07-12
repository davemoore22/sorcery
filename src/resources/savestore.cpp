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

#include "resources/savestore.hpp"
#include "common/types.hpp"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

namespace {

[[noreturn]] auto not_implemented(const std::string_view function) -> void {

	throw std::logic_error{std::string{"SaveStore::"} + std::string{function} +
						   " has not yet been implemented."};
}

}

Sorcery::SaveStore::SaveStore(const std::filesystem::path &game_file,
							  const std::filesystem::path &characters_directory)
	: _game_file{game_file},
	  _characters_directory{characters_directory} {

	if (_game_file.empty())
		throw std::invalid_argument{
			"SaveStore game file path cannot be empty."};

	if (_characters_directory.empty())
		throw std::invalid_argument{
			"SaveStore character directory path cannot be empty."};

	std::error_code error;

	const auto game_directory{_game_file.parent_path()};

	if (!game_directory.empty()) {
		std::filesystem::create_directories(game_directory, error);

		if (error) {
			throw std::filesystem::filesystem_error{
				"Unable to create the game save directory", game_directory,
				error};
		}
	}

	error.clear();

	std::filesystem::create_directories(_characters_directory, error);

	if (error) {
		throw std::filesystem::filesystem_error{
			"Unable to create the character save directory",
			_characters_directory, error};
	}
}

auto Sorcery::SaveStore::has_game() const -> bool {

	std::error_code error;

	const bool exists{std::filesystem::exists(_game_file, error)};

	if (error) {
		throw std::filesystem::filesystem_error{
			"Unable to inspect the game save file", _game_file, error};
	}

	if (!exists)
		return false;

	const bool regular_file{
		std::filesystem::is_regular_file(_game_file, error)};

	if (error) {
		throw std::filesystem::filesystem_error{
			"Unable to inspect the game save file", _game_file, error};
	}

	return regular_file;
}
auto Sorcery::SaveStore::wipe_data() -> void {

	not_implemented("wipe_data");
}

auto Sorcery::SaveStore::create_game_state(std::string data) -> unsigned int {

	static_cast<void>(data);

	not_implemented("create_game_state");
}

auto Sorcery::SaveStore::load_game_state() const -> std::optional<GameEntry> {

	not_implemented("load_game_state");
}

auto Sorcery::SaveStore::save_game_state(const unsigned int game_id,
										 const std::string_view key,
										 std::string data) -> void {

	static_cast<void>(game_id);
	static_cast<void>(key);
	static_cast<void>(data);

	not_implemented("save_game_state");
}

auto Sorcery::SaveStore::add_character(const unsigned int game_id,
									   std::string name, std::string data)
	-> unsigned int {

	static_cast<void>(game_id);
	static_cast<void>(name);
	static_cast<void>(data);

	not_implemented("add_character");
}

auto Sorcery::SaveStore::update_character(const unsigned int game_id,
										  const unsigned int character_id,
										  std::string name, std::string data)
	-> bool {

	static_cast<void>(game_id);
	static_cast<void>(character_id);
	static_cast<void>(name);
	static_cast<void>(data);

	not_implemented("update_character");
}

auto Sorcery::SaveStore::delete_character(const unsigned int game_id,
										  const unsigned int character_id)
	-> void {

	static_cast<void>(game_id);
	static_cast<void>(character_id);

	not_implemented("delete_character");
}

auto Sorcery::SaveStore::get_character_ids(const unsigned int game_id) const
	-> std::vector<unsigned int> {

	static_cast<void>(game_id);

	not_implemented("get_character_ids");
}

auto Sorcery::SaveStore::get_character(const unsigned int game_id,
									   const unsigned int character_id) const
	-> std::string {

	static_cast<void>(game_id);
	static_cast<void>(character_id);

	not_implemented("get_character");
}
