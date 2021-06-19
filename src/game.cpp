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

#include "game.hpp"

Sorcery::Game::Game(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Attempt to load a game from the Database
	characters.clear();
	valid = _system->database->has_game();
	if (valid) {

		// Get the Game
		auto [id, key, status, start_time, last_time] = _system->database->get_game().value();
		_id = id;
		_key = key;
		_status = status;
		_start_time = start_time;
		_last_time = last_time;

		// Load the Characters
		_characters_ids = _system->database->get_character_list(_id);
		characters = load_characters();
	};
}

auto Sorcery::Game::reload_characters() -> void {

	// Load the Characters
	_characters_ids = _system->database->get_character_list(_id);
	characters.clear();
	characters = load_characters();
}

auto Sorcery::Game::start_new_game() -> void {

	// Create a new game no matter what
	_system->database->add_game();
	auto [id, key, status, start_time, last_time] = _system->database->get_game().value();

	_id = id;
	_key = key;
	_status = status;
	_start_time = start_time;
	_last_time = last_time;

	valid = true;
}

auto Sorcery::Game::get_id() -> unsigned int {

	return _id;
}

auto Sorcery::Game::load_characters() -> std::map<unsigned int, Character> {

	std::map<unsigned int, Character> characters;
	characters.clear();

	for (auto character_id : _characters_ids) {

		std::string data{_system->database->get_character(_id, character_id)};
		std::stringstream ss;
		ss.str(data);

		// Remember that the three pointers aren't serialised
		Character character(_system, _display, _graphics);
		character.create_spell_lists();
		{
			cereal::JSONInputArchive archive(ss);
			archive(character);
		}
		character.set_spells();
		character.set_stage(CharacterStage::COMPLETED);
		characters[character_id] = character;
	}

	return characters;
}

auto Sorcery::Game::save_new_character(Character &character) -> unsigned int {

	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		archive(character);
	}
	std::string character_data{ss.str()};

	return _system->database->insert_character(_id, character.get_name(), character_data);
}
