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
#include "engine/include.hpp"

namespace Sorcery {

struct Coordinate3 {

		Coordinate3() : x{0}, y{0}, z{0} {};
		Coordinate3(int x_, int y_, int z_) : x{x_}, y{y_}, z{z_} {};
		Coordinate3(int x_, int z_) : x{x_}, y{0}, z{z_} {};

		auto operator==(const Coordinate3 &a) const -> bool {
			return (x == a.x && y == a.y && z == a.z);
		}

		auto operator<(const Coordinate3 &a) const -> bool {
			return std::tie(x, y, z) < std::tie(a.x, a.y, a.z);
		};

		friend std::ostream &operator<<(std::ostream &os, Coordinate3 const &a) {
			return os << fmt::format("[{}/{}/{}]", a.x, a.y, a.z) << std::endl;
		}

		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(x, y, z);
		}

		int x;
		int y;
		int z;
};

struct TileView {

		Coordinate3 offset;
		sf::VertexArray floor;
		sf::VertexArray ceiling;
		sf::VertexArray up;
		sf::VertexArray down;
		sf::VertexArray darkness;
		sf::VertexArray side_darkness;
		sf::VertexArray back_wall;
		sf::VertexArray back_door;
		sf::VertexArray left_side_wall;
		sf::VertexArray left_side_door;
		sf::VertexArray right_side_wall;
		sf::VertexArray right_side_door;

		TileView() {

			offset = Coordinate3{0, 0, 0};
			floor.setPrimitiveType(sf::Quads);
			floor.resize(4);
			ceiling.setPrimitiveType(sf::Quads);
			ceiling.resize(4);
			up.setPrimitiveType(sf::Quads);
			up.resize(4);
			down.setPrimitiveType(sf::Quads);
			down.resize(4);
			darkness.setPrimitiveType(sf::Quads);
			darkness.resize(4);
			side_darkness.setPrimitiveType(sf::Quads);
			side_darkness.resize(4);
			back_door.setPrimitiveType(sf::Quads);
			back_door.resize(4);
			back_wall.setPrimitiveType(sf::Quads);
			back_wall.resize(4);
			left_side_wall.setPrimitiveType(sf::Quads);
			left_side_wall.resize(4);
			left_side_door.setPrimitiveType(sf::Quads);
			left_side_door.resize(4);
			right_side_wall.setPrimitiveType(sf::Quads);
			right_side_wall.resize(4);
			right_side_door.setPrimitiveType(sf::Quads);
			right_side_door.resize(4);
		};

		TileView(Coordinate3 offset_) : offset{offset_} {

			floor.setPrimitiveType(sf::Quads);
			floor.resize(4);
			ceiling.setPrimitiveType(sf::Quads);
			ceiling.resize(4);
			up.setPrimitiveType(sf::Quads);
			up.resize(4);
			down.setPrimitiveType(sf::Quads);
			down.resize(4);
			darkness.setPrimitiveType(sf::Quads);
			darkness.resize(4);
			side_darkness.setPrimitiveType(sf::Quads);
			side_darkness.resize(4);
			back_door.setPrimitiveType(sf::Quads);
			back_door.resize(4);
			back_wall.setPrimitiveType(sf::Quads);
			back_wall.resize(4);
			left_side_wall.setPrimitiveType(sf::Quads);
			left_side_wall.resize(4);
			left_side_door.setPrimitiveType(sf::Quads);
			left_side_door.resize(4);
			right_side_wall.setPrimitiveType(sf::Quads);
			right_side_wall.resize(4);
			right_side_door.setPrimitiveType(sf::Quads);
			right_side_door.resize(4);
		}
};

}