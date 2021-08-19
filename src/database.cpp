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

#include "database.hpp"

// Standard Constructor
Sorcery::Database::Database(const std::filesystem::path &db_file_path)
	: _db_file_path{db_file_path} {
	try {

		// Attempt to connect to the database to check it is valid
		sqlite::database database(_db_file_path.string());
		const std::string check_valid_db_SQL{"pragma schema_version"};
		database << check_valid_db_SQL >> [&](int return_code) {
			connected = return_code > 0;
		};
	} catch (std::exception &e) {
		connected = false;
	}
}

auto Sorcery::Database::has_game() -> bool {

	try {
		sqlite::database database(_db_file_path.string());

		const std::string check_has_game_SQL{
			"SELECT count(g.id) AS count FROM game g;"};

		int count{};
		database << check_has_game_SQL >> count;

		return count > 0;
	} catch (sqlite::sqlite_exception &e) {
		Error error{SystemError::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_db_file_path.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Database::load_game_state() -> std::optional<GameEntry> {

	try {

		if (has_game()) {

			sqlite::database database(_db_file_path.string());

			const std::string get_game_SQL{
				"SELECT g.id, g.key, g.status, g.started, g.last_played, "
				"g.data FROM game g;"};

			int id{};
			std::string key{};
			std::string started{};
			std::string last_played{};
			std::string status{};
			std::string data{};

			database << get_game_SQL >>
				std::tie(id, key, status, started, last_played, data);

			std::tm started_tm{};
			std::stringstream started_ss(started);
			started_ss >> std::get_time(&started_tm, "%Y-%m-%d %X");
			auto started_tp{std::chrono::system_clock::from_time_t(
				std::mktime(&started_tm))};

			std::tm last_played_tm{};
			std::stringstream last_played_ss(last_played);
			last_played_ss >> std::get_time(&last_played_tm, "%Y-%m-%d %X");
			auto last_played_tp{std::chrono::system_clock::from_time_t(
				std::mktime(&last_played_tm))};

			return GameEntry{static_cast<unsigned int>(id), key, status,
				started_tp, last_played_tp, data};

		} else
			return std::nullopt;

	} catch (sqlite::sqlite_exception &e) {
		Error error{SystemError::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_db_file_path.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Database::save_game_state(
	int game_id, std::string key, std::string data) -> void {
	try {
		sqlite::database database(_db_file_path.string());

		auto now_t{std::chrono::system_clock::to_time_t(
			std::chrono::system_clock::now())};
		std::stringstream ss{};
		ss << std::put_time(std::localtime(&now_t), "%Y-%m-%d %X");
		auto last_played{ss.str()};
		std::string status{"OK"};

		const std::string update_game_SQL{
			"UPDATE game SET status = ?, last_played = ?, data = ? WHERE id = "
			"? AND key = ?;"};
		database << update_game_SQL << status << last_played << data << game_id
				 << key;

	} catch (sqlite::sqlite_exception &e) {
		Error error{SystemError::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_db_file_path.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Database::create_game_state(std::string data) -> unsigned int {

	try {

		sqlite::database database(_db_file_path.string());
		if (has_game()) {

			const std::string delete_existing_game_SQL{"DELETE FROM game;"};
			database << delete_existing_game_SQL;
		}

		std::string new_unique_key{GUID()};
		auto now_t{std::chrono::system_clock::to_time_t(
			std::chrono::system_clock::now())};
		std::stringstream ss{};
		ss << std::put_time(std::localtime(&now_t), "%Y-%m-%d %X");
		auto stated{ss.str()};
		auto last_played{ss.str()};
		std::string status{"OK"};
		const std::string insert_new_game_SQL{
			"INSERT INTO game (key, status, started, last_played, data) VALUES "
			"(?, ?, ?, ?, ?)"};
		database << insert_new_game_SQL << new_unique_key << status << stated
				 << last_played << data;

		return database.last_insert_rowid();

	} catch (sqlite::sqlite_exception &e) {
		Error error{SystemError::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_db_file_path.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}

	return 0;
}

auto Sorcery::Database::update_character(
	int game_id, int character_id, std::string name, std::string data) -> bool {

	try {

		sqlite::database database(_db_file_path.string());

		std::string status{"OK"};
		const std::string update_character_name_SQL{
			"UPDATE CHARACTER SET name = ?, data = ? WHERE game_id = ? AND id "
			"= ?"};

		database << update_character_name_SQL << name << data << game_id
				 << character_id;

		return true;

	} catch (sqlite::sqlite_exception &e) {
		Error error{SystemError::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_db_file_path.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}

	return false;
}

auto Sorcery::Database::add_character(
	int game_id, std::string name, std::string data) -> unsigned int {

	try {

		sqlite::database database(_db_file_path.string());

		auto now_t{std::chrono::system_clock::to_time_t(
			std::chrono::system_clock::now())};
		std::stringstream cs{};
		cs << std::put_time(std::localtime(&now_t), "%Y-%m-%d %X");
		auto created{cs.str()};

		std::string status{"OK"};
		const std::string insert_new_character_SQL{
			"INSERT INTO character (game_id, created, status, name, data) "
			"VALUES (?,?,?,?,?)"};

		database << insert_new_character_SQL << game_id << created << status
				 << name << data;

		return database.last_insert_rowid();

	} catch (sqlite::sqlite_exception &e) {
		Error error{SystemError::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_db_file_path.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}

	return 0;
}

auto Sorcery::Database::get_character_ids(int game_id)
	-> std::vector<unsigned int> {

	std::vector<unsigned int> characters;

	try {

		sqlite::database database(_db_file_path.string());

		const std::string get_character_list_SQL{
			"SELECT c.id FROM character c WHERE c.game_id = ? ORDER BY c.id "
			"ASC;"};

		database << get_character_list_SQL << game_id >> [&](int id) {
			characters.emplace_back(id);
		};

	} catch (sqlite::sqlite_exception &e) {
		Error error{SystemError::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_db_file_path.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}

	return characters;
}

auto Sorcery::Database::delete_character(int game_id, int character_id)
	-> void {

	try {

		sqlite::database database(_db_file_path.string());

		const std::string delete_character_SQL{
			"DELETE FROM character WHERE id = ? AND game_id = ?;"};

		database << delete_character_SQL << character_id << game_id;

	} catch (sqlite::sqlite_exception &e) {
		Error error{SystemError::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_db_file_path.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Database::get_character(int game_id, int character_id)
	-> std::string {

	std::string character_data{};

	try {

		sqlite::database database(_db_file_path.string());

		const std::string get_character_SQL{
			"SELECT c.data FROM character c WHERE c.id = ? AND c.game_id = ?;"};

		database << get_character_SQL << character_id << game_id >>
			character_data;

	} catch (sqlite::sqlite_exception &e) {
		Error error{SystemError::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_db_file_path.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}

	return character_data;
}
