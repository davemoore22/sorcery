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

#include "character.hpp"
#include "explore.hpp"
#include "level.hpp"
#include "main.hpp"
#include "system.hpp"

namespace Sorcery {

	// State holds the changable game data
	class State {

	  public:
		// Constructor
		State();
		State(System *system);

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(_version, _party, level, explored, _player_depth, _player_pos, _playing_facing, _lit);
		}

		// Public Members
		bool valid;
		std::unique_ptr<Level> level; // current level
		std::map<int, Explore> explored;

		// Public Methods
		auto add_character_by_id(unsigned int char_id) -> bool;
		auto check_character_in_party(unsigned int char_id) -> bool;
		auto clear_party() -> void;
		auto get_character_index(unsigned int char_id) -> std::optional<unsigned int>;
		auto get_party_characters() const -> std::vector<unsigned int>;
		auto get_party_size() const -> unsigned int;
		auto get_player_facing() const -> MapDirection;
		auto get_player_pos() const -> Coordinate;
		auto party_has_members() const -> bool;
		auto remove_character_by_id(unsigned int char_id) -> bool;
		auto remove_character_by_position(unsigned int index) -> bool;
		auto set(System *system) -> void;
		auto set_party(std::vector<unsigned int> candidate_party) -> void;
		auto set_current_level(Level *other) -> void;
		auto set_player_facing(const MapDirection direction) -> void;
		auto set_player_pos(const Coordinate position) -> void;
		auto restart_expedition() -> void;
		auto set_depth(int depth) -> void;
		auto get_depth() const -> int;
		auto set_lit(bool lit) -> void;
		auto get_lit() const -> bool;

	  private:
		// Private Methods
		auto _clear() -> void;
		auto _clear_explored() -> void;
		auto _restart_expedition() -> void;

		// Private Members
		System *_system;
		std::vector<unsigned int> _party;
		Coordinate _player_pos;
		MapDirection _playing_facing;
		int _player_depth;
		bool _lit;
		int _version;
	};
} // namespace Sorcery
