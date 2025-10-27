// Copyright (C) 2025 Dave Moore
//
// This file is part of Sorcery.
//
// Sorcery is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 2 of the License, or (at your option) any later
// version.
//
// Sorcery is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Sorcery.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#pragma once

#include "common/include.hpp"
#include "common/types.hpp"

namespace Sorcery {

struct Coordinate3 {

		Coordinate3()
			: x{0},
			  y{0},
			  z{0} {};
		Coordinate3(int x_, int y_, int z_)
			: x{x_},
			  y{y_},
			  z{z_} {};
		Coordinate3(int x_, int z_)
			: x{x_},
			  y{0},
			  z{z_} {};

		auto operator==(const Coordinate3 &a) const -> bool {
			return (x == a.x && y == a.y && z == a.z);
		}

		auto operator<(const Coordinate3 &a) const -> bool {
			return std::tie(x, y, z) < std::tie(a.x, a.y, a.z);
		};

		friend std::ostream &operator<<(std::ostream &os,
										Coordinate3 const &a) {
			return os << std::format("[{}/{}/{}]", a.x, a.y, a.z) << std::endl;
		}

		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(x, y, z);
		}

		int x;
		int y;
		int z;
};

struct Vertex {
		ImVec2 position;
		ImVec4 colour;
		ImVec2 tex_coord;
};

struct VertexArray {

		std::array<Vertex, 4> data;

		VertexArray() = default;

		auto operator[](int index) -> Vertex & {

			return data[index];
		};

		friend std::ostream &operator<<(std::ostream &os,
										VertexArray const &a) {
			return os << std::format(
							 "[p1={}/{}, p2={}/{}, p3={}/{}, p4={}/{}], "
							 "[uv0={}/{}, uv1={}/{}, uv2={}/{}, uv3={}/{}],",
							 a.data[0].position.x, a.data[0].position.y,
							 a.data[1].position.x, a.data[1].position.y,
							 a.data[2].position.x, a.data[2].position.y,
							 a.data[3].position.x, a.data[3].position.y,
							 a.data[0].tex_coord.x, a.data[0].tex_coord.y,
							 a.data[1].tex_coord.x, a.data[1].tex_coord.y,
							 a.data[2].tex_coord.x, a.data[2].tex_coord.y,
							 a.data[3].tex_coord.x, a.data[3].tex_coord.y)
					  << std::endl;
		}
};

struct TileView {

		Coordinate3 offset;
		VertexArray floor;
		VertexArray ceiling;
		VertexArray up;
		VertexArray down;
		VertexArray darkness;
		VertexArray side_darkness;
		VertexArray back_wall;
		VertexArray back_door;
		VertexArray left_side_wall;
		VertexArray left_side_door;
		VertexArray right_side_wall;
		VertexArray right_side_door;

		TileView() {

			offset = Coordinate3{0, 0, 0};
		};

		TileView(Coordinate3 offset_)
			: offset{offset_} {};
};
}