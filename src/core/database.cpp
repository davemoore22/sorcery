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

#include "core/database.hpp"
#include "common/enum.hpp"
#include "common/include.hpp"
#include "common/macro.hpp"
#include "types/error.hpp"

// Standard Constructor
Sorcery::Database::Database(const std::filesystem::path &fp) : _fp{fp} {

	try {

		// Attempt to connect to the database to check it is valid
		sqlite::database database(_fp.string());
		const auto check_SQL{"pragma schema_version"};

		database << check_SQL >>
			[&](int return_code) { connected = return_code > 0; };

	} catch (std::exception &e) {
		connected = false;
	}
}

// Reset and Clean the Database
auto Sorcery::Database::wipe_data() -> void {

	try {

		sqlite::database db(_fp.string());

		const auto delete_char_SQL{"DELETE FROM character"};
		const auto delete_game_SQL{"DELETE FROM game"};
		const auto reset_char_SQL{
			"UPDATE SQLITE_SEQUENCE SET SEQ=0 WHERE NAME='character';"};
		const auto reset_game_SQL{
			"UPDATE SQLITE_SEQUENCE SET SEQ=0 WHERE NAME='game';"};

		db << delete_char_SQL;
		db << delete_game_SQL;
		db << reset_char_SQL;
		db << reset_game_SQL;

	} catch (sqlite::sqlite_exception &e) {
		Error error{SYE::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_fp.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Database::has_game() -> bool {

	try {

		sqlite::database db(_fp.string());

		const auto check_SQL{"SELECT count(g.id) AS count FROM game g;"};
		auto count{0};

		db << check_SQL >> count;

		return count > 0;

	} catch (sqlite::sqlite_exception &e) {
		Error error{SYE::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_fp.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Database::load_game_state() -> std::optional<GameEntry> {

	try {

		if (has_game()) {

			sqlite::database db(_fp.string());

			const auto get_SQL{
				"SELECT g.id, g.key, g.status, g.started, g.last_played, "
				"g.data FROM game g;"};

			auto id{0};
			auto key{""s};
			auto started{""s};
			auto last_played{""s};
			auto status{""s};
			auto data{""s};

			db << get_SQL >>
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
		Error error{SYE::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_fp.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Database::save_game_state(
	int game_id, std::string key, std::string data) -> void {

	try {

		sqlite::database db(_fp.string());

		auto now_t{std::chrono::system_clock::to_time_t(
			std::chrono::system_clock::now())};
		std::stringstream ss{};
		ss << std::put_time(std::localtime(&now_t), "%Y-%m-%d %X");
		auto last_played{ss.str()};
		const auto status{"OK"s};

		const std::string update_SQL{
			"UPDATE game SET status = ?, last_played = ?, data = ? WHERE id = "
			"? AND key = ?;"};
		db << update_SQL << status << last_played << data << game_id << key;

	} catch (sqlite::sqlite_exception &e) {
		Error error{SYE::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_fp.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Database::create_game_state(std::string data) -> unsigned int {

	try {

		sqlite::database db(_fp.string());
		if (has_game()) {

			const std::string delete_SQL{"DELETE FROM game;"};
			db << delete_SQL;
		}

		auto new_unique_key{GUID()};
		auto now_t{std::chrono::system_clock::to_time_t(
			std::chrono::system_clock::now())};
		std::stringstream ss{};
		ss << std::put_time(std::localtime(&now_t), "%Y-%m-%d %X");
		auto stated{ss.str()};
		auto last_played{ss.str()};
		const auto status{"OK"s};
		const std::string insert_SQL{
			"INSERT INTO game (key, status, started, last_played, data) VALUES "
			"(?, ?, ?, ?, ?)"};
		db << insert_SQL << new_unique_key << status << stated << last_played
		   << data;

		return db.last_insert_rowid();

	} catch (sqlite::sqlite_exception &e) {
		Error error{SYE::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_fp.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}

	return 0;
}

auto Sorcery::Database::update_character(
	int game_id, int char_id, std::string name, std::string data) -> bool {

	try {

		sqlite::database db(_fp.string());

		const auto update_SQL{
			"UPDATE CHARACTER SET name = ?, data = ? WHERE game_id = ? AND id "
			"= ?"};

		db << update_SQL << name << data << game_id << char_id;

		return true;

	} catch (sqlite::sqlite_exception &e) {
		Error error{SYE::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_fp.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Database::add_character(
	int game_id, std::string name, std::string data) -> unsigned int {

	try {

		sqlite::database db(_fp.string());

		auto now_t{std::chrono::system_clock::to_time_t(
			std::chrono::system_clock::now())};
		std::stringstream cs{};
		cs << std::put_time(std::localtime(&now_t), "%Y-%m-%d %X");
		auto created{cs.str()};

		const auto status{"OK"s};
		const auto insert_SQL{
			"INSERT INTO character (game_id, created, status, name, data) "
			"VALUES (?,?,?,?,?)"};

		db << insert_SQL << game_id << created << status << name << data;

		return db.last_insert_rowid();

	} catch (sqlite::sqlite_exception &e) {
		Error error{SYE::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_fp.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Database::get_character_ids(int game_id)
	-> std::vector<unsigned int> {

	try {

		sqlite::database db(_fp.string());
		std::vector<unsigned int> characters;

		const auto get_SQL{
			"SELECT c.id FROM character c WHERE c.game_id = ? ORDER BY c.id "
			"ASC;"};

		db << get_SQL << game_id >>
			[&](int id) { characters.emplace_back(id); };

		return characters;

	} catch (sqlite::sqlite_exception &e) {
		Error error{SYE::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_fp.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Database::delete_character(int game_id, int char_id) -> void {

	try {

		sqlite::database db(_fp.string());

		const auto delete_SQL{
			"DELETE FROM character WHERE id = ? AND game_id = ?;"};

		db << delete_SQL << char_id << game_id;

	} catch (sqlite::sqlite_exception &e) {
		Error error{SYE::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_fp.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Database::get_character(int game_id, int char_id) -> std::string {

	try {

		sqlite::database db(_fp.string());

		auto char_data{""s};
		const auto get_SQL{
			"SELECT c.data FROM character c WHERE c.id = ? AND c.game_id = ?;"};

		db << get_SQL << char_id << game_id >> char_data;

		return char_data;

	} catch (sqlite::sqlite_exception &e) {
		Error error{SYE::SQLLITE_ERROR, e,
			fmt::format("{} {} {} {}", e.get_code(), e.what(), e.get_sql(),
				_fp.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}
