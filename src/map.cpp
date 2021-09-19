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

#include "map.hpp"

// Standard Constructor
Sorcery::Map::Map() {

	_version = SAVE_VERSION;

	_reset_level();
	_type = MapType::NONE;
}

Sorcery::Map::Map(MapType type) {

	_type = type;
	_version = SAVE_VERSION;
	_create_level(type);
}

auto Sorcery::Map::get_type() const -> MapType {

	return _type;
}

auto Sorcery::Map::at(Point loc) -> Tile & {

	return _tiles.at(loc.x * MAP_SIZE + loc.y);
}

auto Sorcery::Map::at(unsigned int x, unsigned int y) -> Tile & {

	return _tiles.at(x * MAP_SIZE + y);
}
auto Sorcery::Map::tiles() const -> std::vector<Tile> {

	return _tiles;
}

// Reset the level
auto Sorcery::Map::_reset_level() -> void {

	_type = MapType::NONE;
	_tiles.clear();
	_tiles.reserve(MAP_SIZE * MAP_SIZE);
	for (auto x = 0u; x < 20u; x++) {
		for (auto y = 0u; y < 20u; y++) {
			Tile tile(Point{x, y});
			_tiles.emplace_back(tile);
		}
	}
}

// Reset the level
auto Sorcery::Map::_create_level(MapType type) -> void {

	_type = type;
	switch (_type) {
	case MapType::EMPTY:
	case MapType::START:

		// A simple enclosed blank level
		_tiles.clear();
		_tiles.reserve(MAP_SIZE * MAP_SIZE);
		for (auto x = 0u; x < 20u; x++) {
			for (auto y = 0u; y < 20u; y++) {
				Tile tile(Point{x, y});
				tile.set_walkable();
				_tiles.emplace_back(tile);
			}
		}
		for (auto x = 0u; x < 20u; x++) {
			auto tile_left{_tiles.at(x * MAP_SIZE + 0)};
			tile_left.set_walls(false, false, false, true);

			auto tile_right{_tiles.at(x * MAP_SIZE + 19)};
			tile_right.set_walls(false, false, true, false);
		}

		for (auto y = 0u; y < 20u; y++) {
			auto tile_top{_tiles.at(y)};
			tile_top.set_walls(true, false, false, false);

			auto tile_bottom{_tiles.at(y + MAP_SIZE * 19)};
			tile_bottom.set_walls(false, true, false, false);
		}

		break;
	default:
		_reset_level();
		break;
	}

	if (_type == MapType::START) {
		for (auto x = 0u; x < 20u; x++) {
			for (auto y = 0u; y < 20u; y++) {
				auto tile{_tiles.at(x * MAP_SIZE + y)};
				tile.set_gfx(1);
			}
		}
	}
}
