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

#pragma once

#include "main.hpp"
#include "tile.hpp"

namespace Sorcery {

	class Level {

	  public:
		// Constructors
		Level();
		Level(const MapType type, const std::string dungeon, const int depth,
			const Coordinate bottom_left, const Size size);

		// Copy Constructors
		Level(const Level &other);
		auto operator=(const Level &other) -> Level &;

		// Overload [] operator
		auto operator[](Coordinate loc) -> Tile_ &;

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(_type, _dungeon, _depth, _bottom_left, _size, _tiles);
		}

		// Public Members

		// Public Methods
		auto at(const Coordinate loc) -> Tile_ &;
		auto at(const int x, const int y) -> Tile_ &;
		auto bottom_left() const -> Coordinate;
		auto depth() const -> int;
		auto in(const Coordinate loc) const -> bool;
		auto load(const Json::Value row_data) -> bool;
		auto name() const -> std::string;
		auto set(const Level *other) -> void;
		auto size() const -> Size;
		auto top_right() const -> Coordinate;
		auto type() const -> MapType;

	  private:
		// Private Members - need getters for these (bot not setters)
		MapType _type;
		std::string _dungeon;
		int _depth;
		Coordinate _bottom_left;
		Size _size;
		std::map<Coordinate, Tile_> _tiles;

		// Private Methods
		auto _add_tile(const Coordinate location) -> void;
		auto _convert_edge(const unsigned int wall) const
			-> std::optional<TileEdge>;
		auto _convert_edge_se(const unsigned int wall) const -> TileEdge;
		auto _create() -> void;
		auto _convert_edge_nw(const unsigned int wall) const -> TileEdge;
		auto _load_first_pass(const Json::Value row_data) -> bool;
		auto _load_second_pass(const Json::Value row_data) -> bool;
		auto _update_tile(const Coordinate location,
			const unsigned int north_wall, const unsigned int west_wall)
			-> void;
		auto _update_tile(const Coordinate location,
			const unsigned int south_wall, const unsigned int east_wall,
			const bool darkness, const unsigned int marker,
			const unsigned int terrain) -> void;
	};

} // namespace Sorcery