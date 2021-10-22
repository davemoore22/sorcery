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

#include "world.hpp"

// Standard Constructor
Sorcery::World::World() {

	current_level = std::make_unique<Map>();
	_version = SAVE_VERSION;
	playing_facing = MapDirection::NORTH;
	player_pos = Point{0, 0};
}

auto Sorcery::World::create() -> void {

	current_level = std::make_unique<Map>(MapType::START);
	playing_facing = MapDirection::NORTH;
	player_pos = Point{0, 0};
}

auto Sorcery::World::_get_visible_tiles() -> std::vector<Tile*> {
	//

	/*

	Player view (0,0 is P):

	x/z

	   0 -1 -2 -3 -4 -5 -6

	-6                   O
	-5                O  O
	-4             O  O  O
	-3       O  O  O  O  O
	-2    O  O  O  O  O  O
	-1 O  O  O  O  O  O  O
	 0 P  O  O  O  O  O  O
	+1 O  O  O  O  O  O  O
	+2    O  O  O  O  O  O
	+3       O  O  O  O  O
	+4             O  O  O
	+5                O  O
	+6                   O

	*/
}
