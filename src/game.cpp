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

#include "game.hpp"

Sorcery::Game::Game(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	valid = _system->database->has_game();
	if (valid) {
		_clear();
		_load_game();
	} else {
		_create_game();
		_load_game();
	}
	_load_levels();
}

auto Sorcery::Game::get_id() -> unsigned int {

	return _id;
}

auto Sorcery::Game::create_game() -> void {

	_create_game();
}

// Note that the definitions are fixed!
auto Sorcery::Game::_load_levels() -> void {

	_levels =
		std::make_unique<LevelStore>(_system, (*_system->files)[LEVELS_FILE]);
}

auto Sorcery::Game::load_game() -> void {

	_load_game();
}

auto Sorcery::Game::save_game() -> void {

	_save_game();
}

auto Sorcery::Game::delete_character(unsigned int character_id) -> void {

	_system->database->delete_character(_id, character_id);
}

auto Sorcery::Game::_clear() -> void {

	// Clear existing data!
	if (state.get()) {
		state.release();
		state.reset();
	}

	characters.clear();
	_characters_ids.clear();
	state = std::make_unique<State>(_system);
	state->world->create();
}

auto Sorcery::Game::_create_game() -> void {

	_clear();
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		archive(state);
	}
	const auto data{ss.str()};
	_system->database->create_game_state(data);
}

auto Sorcery::Game::_load_game() -> void {

	// Get Game and State Data
	auto [id, key, status, start_time, last_time, data] =
		_system->database->load_game_state().value();
	_id = id;
	_key = key;
	_status = status;
	_start_time = start_time;
	_last_time = last_time;
	state = std::make_unique<State>(_system);
	if (data.length() > 0) {
		std::stringstream ss;
		ss.str(data);
		{
			cereal::JSONInputArchive archive(ss);
			archive(state);
			state->set(_system);
		}
	}

	// And load the associated characters
	_load_characters();
}

auto Sorcery::Game::_save_game() -> void {

	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		archive(state);
	}
	const auto data{ss.str()};

	_system->database->save_game_state(_id, _key, data);
	_save_characters();
}

auto Sorcery::Game::_save_characters() -> void {

	for (const auto &[character_id, character] : characters) {

		std::stringstream ss;
		{
			cereal::JSONOutputArchive archive(ss);
			archive(character);
		}
		const auto character_data{ss.str()};

		_system->database->update_character(
			_id, character_id, character.get_name(), character_data);
	}
}

auto Sorcery::Game::_get_characters() -> std::map<unsigned int, Character> {

	std::map<unsigned int, Character> characters;
	characters.clear();

	for (auto character_id : _characters_ids) {

		const auto data{_system->database->get_character(_id, character_id)};
		std::stringstream ss;
		ss.str(data);

		// Remember that the three pointers aren't serialised
		Character character(_system, _display, _graphics);
		character.create_spells();
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

auto Sorcery::Game::add_character(Character &character) -> unsigned int {

	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		archive(character);
	}
	const auto character_data{ss.str()};

	return _system->database->add_character(
		_id, character.get_name(), character_data);
}

auto Sorcery::Game::update_character(unsigned int game_id,
	unsigned int character_id, Character &character) -> bool {
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		archive(character);
	}
	const auto character_data{ss.str()};

	return _system->database->update_character(
		game_id, character_id, character.get_name(), character_data);
}

auto Sorcery::Game::_load_characters() -> void {

	_characters_ids.clear();
	_characters_ids = _system->database->get_character_ids(_id);
	characters.clear();
	characters = _get_characters();
}
