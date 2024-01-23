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

#pragma once

#include "common/include.hpp"
#include "common/type.hpp"

namespace Sorcery {

struct Elevator {
		bool up;
		Coordinate up_loc;
		bool down;
		Coordinate down_loc;
		int top_depth;
		int bottom_depth;

		Elevator()
			: up{false}, up_loc{Coordinate{0, 0}}, down{false}, down_loc{Coordinate{0, 0}}, top_depth{0},
			  bottom_depth{0} {};

		Elevator(bool up_, Coordinate up_loc_, bool down_, Coordinate down_loc_, int top_depth_, int bottom_depth_)
			: up{up_}, up_loc{up_loc_}, down{down_}, down_loc{down_loc_}, top_depth{top_depth_},
			  bottom_depth{bottom_depth_} {};

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(up, up_loc, down_loc, top_depth, bottom_depth);
		}
};

struct Teleport {
		int to_level;
		Coordinate to_loc;

		Teleport() : to_level{0}, to_loc{Coordinate{0, 0}} {};

		Teleport(int to_level_, Coordinate to_loc_) : to_level{to_level_}, to_loc{to_loc_} {
		}

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(to_level, to_loc);
		}
};

}