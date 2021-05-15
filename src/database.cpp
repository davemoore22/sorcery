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
		const std::string check_has_game_SQL{"SELECT count(g.id) AS count FROM game g;"};
		int count{};
		database << check_has_game_SQL >> count;
		return count > 0;
	} catch (sqlite::sqlite_exception &e) {
		Error error{SystemError::SQLLITE_ERROR, e,
			fmt::format(
				"{} {} {} {}", e.get_code(), e.what(), e.get_sql(), _db_file_path.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Database::get_game() -> std::optional<GameEntry> {

	if (has_game()) {

		sqlite::database database(_db_file_path.string());
		const std::string get_game_SQL{
			"SELECT g.id, g.key, g.status, g.started, g.last_played FROM game;"};

		int id{};
		std::string key{};
		std::string started{};
		std::string last_played{};
		std::string status{};
		database << get_game_SQL >> std::tie(id, key, status, started, last_played);

		std::tm started_tm{};
		std::stringstream started_ss(started);
		started_ss >> std::get_time(&started_tm, "%Y-%m-%d %X");
		auto started_tp{std::chrono::system_clock::from_time_t(std::mktime(&started_tm))};

		std::tm last_played_tm{};
		std::stringstream last_played_ss(last_played);
		last_played_ss >> std::get_time(&last_played_tm, "%Y-%m-%d %X");
		auto last_played_tp{std::chrono::system_clock::from_time_t(std::mktime(&last_played_tm))};

		return std::make_tuple(
			static_cast<unsigned int>(id), key, status, started_tp, last_played_tp);

	} else
		return std::nullopt;
}

auto Sorcery::Database::add_game() -> int {
	try {

		sqlite::database database(_db_file_path.string());
		if (has_game()) {

			const std::string delete_existing_game_SQL{"DELETE FROM game;"};
			database << delete_existing_game_SQL;
		}

		std::string new_unique_key{GUID()};
		auto now_t{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
		std::stringstream ss{};
		ss << std::put_time(std::localtime(&now_t), "%Y-%m-%d %X");
		auto stated{ss.str()};
		auto last_played{ss.str()};
		std::string status{"OK"};
		const std::string insert_new_game_SQL{
			"INSERT INTO game (key, status, started, last_played) VALUES (?,?,?,?)"};
		database << insert_new_game_SQL << new_unique_key << status << stated << last_played;

		return database.last_insert_rowid();

	} catch (sqlite::sqlite_exception &e) {
		Error error{SystemError::SQLLITE_ERROR, e,
			fmt::format(
				"{} {} {} {}", e.get_code(), e.what(), e.get_sql(), _db_file_path.string())};
		std::cout << error;
		exit(EXIT_FAILURE);
	}

	return 0;
}

// Get the Character List
auto Sorcery::Database::get_character_list() -> std::vector<CharacterList> {

	std::vector<CharacterList> character_list;
	sqlite::database database(_db_file_path.string());

	const std::string get_character_list_SQL{
		"SELECT"
		"	g.id AS game_id, "
		"	g.name AS game_name, "
		"	g.status AS game_status, "
		"	c.id AS character_id, "
		"	c.status AS character_status, "
		"	c.created AS character_created, "
		"	c.name AS character_name ",
		"	c.current_level AS character_level, "
		"	c.current_class AS character_class "
		"FROM "
		"	character c "
		"LEFT OUTER JOIN game g ON "
		"	c.game_id = g.id "
		"ORDER BY "
		"	g.id, "
		"	c.created, "
		"	c.status;"};
	database << get_character_list_SQL >>
		[&](std::string game_id, std::string game_name, unsigned int game_status,
			unsigned int character_id, unsigned int character_status, std::string character_created,
			std::string character_name) {
			character_list.push_back(std::tuple<std::string, std::string, unsigned int,
				unsigned int, unsigned int, std::string, std::string>(game_id, game_name,
				game_status, character_id, character_status, character_created, character_name));
		};

	return character_list;
}
