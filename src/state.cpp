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

#include "state.hpp"

// Constructor used by Cereal to serialise this item
Sorcery::State::State() {
	_party.clear();
	_party.reserve(MAX_PARTY_SIZE);
}

// Normal Constructor
Sorcery::State::State(System *system) : _system{system} {

	_party.clear();
	_party.reserve(MAX_PARTY_SIZE);
}

// Method called to simulate Normal Constructor with Cereal Constructor
auto Sorcery::State::set(System *system) -> void {

	_system = system;
}

auto Sorcery::State::set_party(std::vector<unsigned int> candidate_party)
	-> void {

	_party = candidate_party;
}

auto Sorcery::State::party_has_members() -> bool {

	return _party.size() > 0;
}

auto Sorcery::State::get_party_characters() -> std::vector<unsigned int> {

	return _party;
}

auto Sorcery::State::clear_party() -> void {

	_party.clear();
}

auto Sorcery::State::add_character_by_id(unsigned int char_id) -> bool {

	if (_party.size() < MAX_PARTY_SIZE) {
		_party.push_back(char_id);
		return true;
	} else
		return false;
}

auto Sorcery::State::check_character_in_party(unsigned int char_id) -> bool {

	if (_party.size() > 0) {
		auto found{
			std::find_if(_party.begin(), _party.end(), [&](unsigned int id) {
				return id = char_id;
			})};
		return found != std::end(_party);
	} else
		return false;
}

auto Sorcery::State::get_character_index(unsigned int char_id)
	-> std::optional<unsigned int> {

	if (_party.size() > 0) {

		auto distance{
			std::find_if(_party.begin(), _party.end(), [&](unsigned int id) {
				return id == char_id;
			})};
		if (distance != _party.end())
			return std::distance(_party.begin(), distance);
		else
			return std::nullopt;
	} else
		return std::nullopt;
}

auto Sorcery::State::remove_character_by_id(unsigned int char_id) -> bool {

	if (_party.size() > 0) {
		_party.erase(std::remove_if(_party.begin(), _party.end(),
						 [&](unsigned int id) {
							 return id == char_id;
						 }),
			_party.end());
		return true;
	} else
		return false;
}

auto Sorcery::State::remove_character_by_position(unsigned int index) -> bool {

	if (_party.size() > index) {
		_party.erase(_party.begin() + index);
		return true;
	} else
		return false;
}
