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
#include "common/cereal.hpp"
#include "common/types.hpp"
#include "core/debug.hpp"

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

struct CharacterSaveRecord {
		std::uint32_t version{1};
		unsigned int id{};
		unsigned int game_id{};
		std::string name;
		std::string status;
		std::int64_t created{};
		std::string data;

		template <class Archive> auto serialize(Archive &archive) -> void {

			archive(CEREAL_NVP(version), CEREAL_NVP(id), CEREAL_NVP(game_id),
					CEREAL_NVP(name), CEREAL_NVP(status), CEREAL_NVP(created),
					CEREAL_NVP(data));
		}
};

struct GameSaveRecord {
		std::uint32_t version{1};
		unsigned int id{1};
		std::string key;
		std::string status;
		std::int64_t started{};
		std::int64_t last_played{};
		std::string data;
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(CEREAL_NVP(version), CEREAL_NVP(id), CEREAL_NVP(key),
					CEREAL_NVP(status), CEREAL_NVP(started),
					CEREAL_NVP(last_played), CEREAL_NVP(data));
		}
};

}

Sorcery::SaveStore::SaveStore(const std::filesystem::path &game_file,
							  const std::filesystem::path &characters_directory)
	: _game_file{game_file},
	  _characters_directory{characters_directory} {

	DEBUG_LOGF(
		"SaveStore::SaveStore(game_file='{}', characters_directory='{}')",
		_game_file.string(), _characters_directory.string());

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

	DEBUG_LOGF("SaveStore::has_game(game_file='{}')", _game_file.string());

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

	DEBUG_LOGF(
		"SaveStore::wipe_data(game_file='{}', characters_directory='{}')",
		_game_file.string(), _characters_directory.string());

	std::error_code error;

	// Remove the active game save, if present.
	std::filesystem::remove(_game_file, error);

	if (error) {
		throw std::filesystem::filesystem_error{
			"Unable to remove the game save file", _game_file, error};
	}

	error.clear();

	if (!std::filesystem::exists(_characters_directory, error)) {
		if (error) {
			throw std::filesystem::filesystem_error{
				"Unable to inspect the character save directory",
				_characters_directory, error};
		}

		return;
	}

	if (!std::filesystem::is_directory(_characters_directory, error)) {
		if (error) {
			throw std::filesystem::filesystem_error{
				"Unable to inspect the character save directory",
				_characters_directory, error};
		}

		throw std::runtime_error{"Character save path is not a directory: " +
								 _characters_directory.string()};
	}

	std::filesystem::directory_iterator iterator{_characters_directory, error};

	if (error) {
		throw std::filesystem::filesystem_error{
			"Unable to open the character save directory",
			_characters_directory, error};
	}

	const std::filesystem::directory_iterator end;

	while (iterator != end) {
		const auto entry_path{iterator->path()};

		error.clear();

		const bool regular_file{iterator->is_regular_file(error)};

		if (error) {
			throw std::filesystem::filesystem_error{
				"Unable to inspect character save entry", entry_path, error};
		}

		if (regular_file && entry_path.extension() == ".json") {
			const std::string stem{entry_path.stem().string()};

			unsigned int character_id{};

			const auto [ptr, conversion_error]{std::from_chars(
				stem.data(), stem.data() + stem.size(), character_id)};

			const bool valid_character_file{conversion_error == std::errc{} &&
											ptr == stem.data() + stem.size() &&
											character_id > 0};

			if (valid_character_file) {
				error.clear();

				std::filesystem::remove(entry_path, error);

				if (error) {
					throw std::filesystem::filesystem_error{
						"Unable to remove character save file", entry_path,
						error};
				}
			}
		}

		iterator.increment(error);

		if (error) {
			throw std::filesystem::filesystem_error{
				"Unable to enumerate the character save directory",
				_characters_directory, error};
		}
	}
}

auto Sorcery::SaveStore::create_game_state(std::string key, std::string data)
	-> unsigned int {

	DEBUG_LOGF(
		"SaveStore::create_game_state(game_file='{}', "
		"characters_directory='{}')",
		_game_file.string(), _characters_directory.string());

	constexpr unsigned int game_id{1};
	const auto now{std::chrono::system_clock::now()};
	GameSaveRecord game{.version = 1,
						.id = game_id,
						.key = key,
						.status = "OK",
						.started = _to_epoch_seconds(now),
						.last_played = _to_epoch_seconds(now),
						.data = std::move(data)};
	const std::filesystem::path temporary_file{_game_file.string() + ".tmp"};
	try {
		const auto parent_directory{_game_file.parent_path()};
		if (!parent_directory.empty())
			std::filesystem::create_directories(parent_directory);
		{
			std::ofstream output{temporary_file,
								 std::ios::out | std::ios::trunc};
			if (!output.is_open()) {
				throw std::runtime_error{
					"Unable to open temporary game save file: " +
					temporary_file.string()};
			}
			cereal::JSONOutputArchive archive{output};
			archive(cereal::make_nvp("game", game));
			output.flush();
			if (!output) {
				throw std::runtime_error{
					"Unable to write temporary game save file: " +
					temporary_file.string()};
			}
		}
		std::error_code error;
		std::filesystem::rename(temporary_file, _game_file, error);
		if (error) { /* * Some platforms do not replace an existing destination
						during * rename, so remove the old save and retry. */
			error.clear();
			std::filesystem::remove(_game_file, error);
			if (error) {
				throw std::filesystem::filesystem_error{
					"Unable to replace the existing game save file", _game_file,
					error};
			}
			std::filesystem::rename(temporary_file, _game_file, error);
			if (error) {
				throw std::filesystem::filesystem_error{
					"Unable to install the new game save file", temporary_file,
					_game_file, error};
			}
		}
		return game_id;
	} catch (...) {
		std::error_code ignored_error;
		std::filesystem::remove(temporary_file, ignored_error);
		throw;
	}
}

auto Sorcery::SaveStore::load_game_state() const -> std::optional<GameEntry> {

	DEBUG_LOGF(
		"SaveStore::load_game_state(game_file='{}', "
		"characters_directory='{}')",
		_game_file.string(), _characters_directory.string());

	if (!has_game())
		return std::nullopt;

	try {
		std::ifstream input{_game_file, std::ios::in};

		if (!input.is_open()) {
			throw std::runtime_error{"Unable to open game save file: " +
									 _game_file.string()};
		}

		GameSaveRecord game;

		{
			cereal::JSONInputArchive archive{input};

			archive(cereal::make_nvp("game", game));
		}

		if (!input.eof() && input.fail()) {
			throw std::runtime_error{"Unable to read game save file: " +
									 _game_file.string()};
		}

		if (game.version != 1) {
			throw std::runtime_error{"Unsupported game save version " +
									 std::to_string(game.version) +
									 " in file: " + _game_file.string()};
		}

		if (game.id == 0) {
			throw std::runtime_error{"Invalid game ID in save file: " +
									 _game_file.string()};
		}

		if (game.key.empty()) {
			throw std::runtime_error{"Missing game key in save file: " +
									 _game_file.string()};
		}

		return GameEntry{game.id,
						 std::move(game.key),
						 std::move(game.status),
						 _from_epoch_seconds(game.started),
						 _from_epoch_seconds(game.last_played),
						 std::move(game.data)};

	} catch (const cereal::Exception &error) {
		throw std::runtime_error{"Unable to deserialize game save file '" +
								 _game_file.string() + "': " + error.what()};
	}
}

auto Sorcery::SaveStore::save_game_state(const unsigned int game_id,
										 const std::string_view key,
										 std::string data) -> void {

	DEBUG_LOGF(
		"SaveStore::save_game_state(game_file='{}', characters_directory='{}')",
		_game_file.string(), _characters_directory.string());

	if (!has_game()) {
		throw std::runtime_error{
			"Unable to update game state because no game save exists: " +
			_game_file.string()};
	}

	GameSaveRecord game;

	try {
		std::ifstream input{_game_file, std::ios::in};

		if (!input.is_open()) {
			throw std::runtime_error{"Unable to open game save file: " +
									 _game_file.string()};
		}

		cereal::JSONInputArchive archive{input};

		archive(cereal::make_nvp("game", game));

	} catch (const cereal::Exception &error) {
		throw std::runtime_error{"Unable to deserialize game save file '" +
								 _game_file.string() + "': " + error.what()};
	}

	if (game.version != 1) {
		throw std::runtime_error{"Unsupported game save version " +
								 std::to_string(game.version) +
								 " in file: " + _game_file.string()};
	}

	if (game.id != game_id) {
		throw std::runtime_error{
			"Game ID does not match the active save file."};
	}

	if (game.key != key) {
		throw std::runtime_error{
			"Game key does not match the active save file."};
	}

	game.status = "OK";
	game.last_played = _to_epoch_seconds(std::chrono::system_clock::now());
	game.data = std::move(data);

	const std::filesystem::path temporary_file{_game_file.string() + ".tmp"};

	try {
		{
			std::ofstream output{temporary_file,
								 std::ios::out | std::ios::trunc};

			if (!output.is_open()) {
				throw std::runtime_error{
					"Unable to open temporary game save file: " +
					temporary_file.string()};
			}

			cereal::JSONOutputArchive archive{output};

			archive(cereal::make_nvp("game", game));

			output.flush();

			if (!output) {
				throw std::runtime_error{
					"Unable to write temporary game save file: " +
					temporary_file.string()};
			}
		}

		std::error_code error;

		std::filesystem::rename(temporary_file, _game_file, error);

		if (error) {
			error.clear();

			std::filesystem::remove(_game_file, error);

			if (error) {
				throw std::filesystem::filesystem_error{
					"Unable to replace the existing game save file", _game_file,
					error};
			}

			std::filesystem::rename(temporary_file, _game_file, error);

			if (error) {
				throw std::filesystem::filesystem_error{
					"Unable to install the updated game save file",
					temporary_file, _game_file, error};
			}
		}

	} catch (...) {
		std::error_code ignored_error;

		std::filesystem::remove(temporary_file, ignored_error);

		throw;
	}
}

auto Sorcery::SaveStore::add_character(const unsigned int game_id,
									   std::string name, std::string data)
	-> unsigned int {

	DEBUG_LOGF(
		"SaveStore::add_character(game_file='{}', characters_directory='{}')",
		_game_file.string(), _characters_directory.string());

	if (!has_game()) {
		throw std::runtime_error{
			"Cannot create a character because no game exists."};
	}

	const auto character_ids{get_character_ids(game_id)};

	unsigned int character_id{1};

	if (!character_ids.empty())
		character_id = character_ids.back() + 1;

	const CharacterSaveRecord character{
		.version = 1,
		.id = character_id,
		.game_id = game_id,
		.name = std::move(name),
		.status = "OK",
		.created = _to_epoch_seconds(std::chrono::system_clock::now()),
		.data = std::move(data)};

	const auto character_file{_characters_directory /
							  (std::to_string(character_id) + ".json")};

	const auto temporary_file{character_file.string() + ".tmp"};

	try {

		{
			std::ofstream output{temporary_file,
								 std::ios::out | std::ios::trunc};

			if (!output.is_open()) {
				throw std::runtime_error{
					"Unable to open temporary character save: " +
					temporary_file};
			}

			cereal::JSONOutputArchive archive{output};

			archive(cereal::make_nvp("character", character));
		}

		std::error_code error;

		std::filesystem::rename(temporary_file, character_file, error);

		if (error) {
			throw std::filesystem::filesystem_error{
				"Unable to install character save", temporary_file,
				character_file, error};
		}

		return character_id;

	} catch (...) {

		std::error_code ignored;

		std::filesystem::remove(temporary_file, ignored);

		throw;
	}
}

auto Sorcery::SaveStore::update_character(const unsigned int game_id,
										  const unsigned int character_id,
										  std::string name, std::string data)
	-> bool {

	DEBUG_LOGF(
		"SaveStore::update_character(game_file='{}', "
		"characters_directory='{}')",
		_game_file.string(), _characters_directory.string());

	const auto character_file{_characters_directory /
							  (std::to_string(character_id) + ".json")};

	std::error_code error;

	if (!std::filesystem::is_regular_file(character_file, error)) {
		if (error) {
			throw std::filesystem::filesystem_error{
				"Unable to inspect character save file", character_file, error};
		}

		return false;
	}

	CharacterSaveRecord character;

	try {
		std::ifstream input{character_file, std::ios::in};

		if (!input.is_open()) {
			throw std::runtime_error{"Unable to open character save file: " +
									 character_file.string()};
		}

		cereal::JSONInputArchive archive{input};

		archive(cereal::make_nvp("character", character));

	} catch (const cereal::Exception &error) {
		throw std::runtime_error{"Unable to deserialize character save file '" +
								 character_file.string() +
								 "': " + error.what()};
	}

	if (character.version != 1) {
		throw std::runtime_error{"Unsupported character save version " +
								 std::to_string(character.version) +
								 " in file: " + character_file.string()};
	}

	if (character.id != character_id) {
		throw std::runtime_error{
			"Character ID does not match its save filename: " +
			character_file.string()};
	}

	if (character.game_id != game_id)
		return false;

	character.name = std::move(name);
	character.data = std::move(data);
	character.status = "OK";

	const auto temporary_file{
		std::filesystem::path{character_file.string() + ".tmp"}};

	try {
		{
			std::ofstream output{temporary_file,
								 std::ios::out | std::ios::trunc};

			if (!output.is_open()) {
				throw std::runtime_error{
					"Unable to open temporary character save file: " +
					temporary_file.string()};
			}

			cereal::JSONOutputArchive archive{output};

			archive(cereal::make_nvp("character", character));

			output.flush();

			if (!output) {
				throw std::runtime_error{
					"Unable to write temporary character save file: " +
					temporary_file.string()};
			}
		}

		error.clear();

		std::filesystem::rename(temporary_file, character_file, error);

		if (error) {
			error.clear();

			std::filesystem::remove(character_file, error);

			if (error) {
				throw std::filesystem::filesystem_error{
					"Unable to replace the existing character save file",
					character_file, error};
			}

			std::filesystem::rename(temporary_file, character_file, error);

			if (error) {
				throw std::filesystem::filesystem_error{
					"Unable to install the updated character save file",
					temporary_file, character_file, error};
			}
		}

		return true;

	} catch (...) {
		std::error_code ignored_error;

		std::filesystem::remove(temporary_file, ignored_error);

		throw;
	}
}

auto Sorcery::SaveStore::delete_character(const unsigned int game_id,
										  const unsigned int character_id)
	-> void {

	DEBUG_LOGF(
		"SaveStore::delete_character(game_file='{}', "
		"characters_directory='{}')",
		_game_file.string(), _characters_directory.string());

	const auto character_file{_characters_directory /
							  (std::to_string(character_id) + ".json")};

	std::error_code error;

	if (!std::filesystem::exists(character_file, error)) {
		if (error) {
			throw std::filesystem::filesystem_error{
				"Unable to inspect character save file", character_file, error};
		}

		return;
	}

	CharacterSaveRecord character;

	try {

		std::ifstream input{character_file, std::ios::in};

		if (!input.is_open()) {
			throw std::runtime_error{"Unable to open character save file: " +
									 character_file.string()};
		}

		cereal::JSONInputArchive archive{input};

		archive(cereal::make_nvp("character", character));

	} catch (const cereal::Exception &error) {

		throw std::runtime_error{"Unable to deserialize character save file '" +
								 character_file.string() +
								 "': " + error.what()};
	}

	if (character.game_id != game_id)
		return;

	error.clear();

	std::filesystem::remove(character_file, error);

	if (error) {
		throw std::filesystem::filesystem_error{
			"Unable to remove character save file", character_file, error};
	}
}

auto Sorcery::SaveStore::get_character_ids(const unsigned int game_id) const
	-> std::vector<unsigned int> {

	DEBUG_LOGF(
		"SaveStore::get_character_ids(game_file='{}', "
		"characters_directory='{}')",
		_game_file.string(), _characters_directory.string());

	// There is currently only one active game, so character files all belong
	// to that game
	static_cast<void>(game_id);

	std::vector<unsigned int> character_ids;

	std::error_code error;

	if (!std::filesystem::exists(_characters_directory, error)) {
		if (error) {
			throw std::filesystem::filesystem_error{
				"Unable to inspect the character save directory",
				_characters_directory, error};
		}

		return character_ids;
	}

	if (!std::filesystem::is_directory(_characters_directory, error)) {
		if (error) {
			throw std::filesystem::filesystem_error{
				"Unable to inspect the character save directory",
				_characters_directory, error};
		}

		throw std::runtime_error{"Character save path is not a directory: " +
								 _characters_directory.string()};
	}

	std::filesystem::directory_iterator iterator{_characters_directory, error};

	if (error) {
		throw std::filesystem::filesystem_error{
			"Unable to open the character save directory",
			_characters_directory, error};
	}

	const std::filesystem::directory_iterator end;
	while (iterator != end) {
		const auto &entry{*iterator};

		error.clear();

		if (entry.is_regular_file(error) &&
			entry.path().extension() == ".json") {

			const std::string stem{entry.path().stem().string()};

			unsigned int character_id{};

			const auto [ptr, conversion_error]{std::from_chars(
				stem.data(), stem.data() + stem.size(), character_id)};

			/*
			 * Accept only complete positive integer filenames:
			 *
			 *     1.json      valid
			 *     12.json     valid
			 *     0.json      ignored
			 *     1-old.json  ignored
			 *     fred.json   ignored
			 */
			if (conversion_error == std::errc{} &&
				ptr == stem.data() + stem.size() && character_id > 0) {

				character_ids.emplace_back(character_id);
			}
		}

		if (error) {
			throw std::filesystem::filesystem_error{
				"Unable to inspect a character save entry", entry.path(),
				error};
		}

		iterator.increment(error);

		if (error) {
			throw std::filesystem::filesystem_error{
				"Unable to enumerate the character save directory",
				_characters_directory, error};
		}
	}

	std::ranges::sort(character_ids);

	return character_ids;
}

auto Sorcery::SaveStore::get_character(const unsigned int game_id,
									   const unsigned int character_id) const
	-> std::string {

	DEBUG_LOGF(
		"SaveStore::get_character(game_file='{}', "
		"characters_directory='{}')",
		_game_file.string(), _characters_directory.string());

	const auto character_file{_characters_directory /
							  (std::to_string(character_id) + ".json")};

	std::error_code error;

	if (!std::filesystem::is_regular_file(character_file, error)) {
		if (error) {
			throw std::filesystem::filesystem_error{
				"Unable to inspect character save file", character_file, error};
		}

		return {};
	}

	CharacterSaveRecord character;

	try {

		std::ifstream input{character_file, std::ios::in};

		if (!input.is_open()) {
			throw std::runtime_error{"Unable to open character save file: " +
									 character_file.string()};
		}

		cereal::JSONInputArchive archive{input};

		archive(cereal::make_nvp("character", character));

	} catch (const cereal::Exception &error) {

		throw std::runtime_error{"Unable to deserialize character save file '" +
								 character_file.string() +
								 "': " + error.what()};
	}

	if (character.game_id != game_id)
		return {};

	return character.data;
}

auto Sorcery::SaveStore::_to_epoch_seconds(
	const std::chrono::system_clock::time_point time) const -> std::int64_t {

	return std::chrono::duration_cast<std::chrono::seconds>(
			   time.time_since_epoch())
		.count();
}

auto Sorcery::SaveStore::_from_epoch_seconds(const std::int64_t seconds) const
	-> std::chrono::system_clock::time_point {

	return std::chrono::system_clock::time_point{std::chrono::seconds{seconds}};
}