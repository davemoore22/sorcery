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

	return _tiles.at(COORD2VECPOS(loc.x, loc.y));
}

auto Sorcery::Map::at_relative(Point player_pos, int x, int y) -> Tile & {

	const auto n_x{static_cast<int>(player_pos.x) + x};
	const auto n_y{static_cast<int>(player_pos.y) + y};
	const auto new_x{[&] {
		if (n_x > MAP_SIZE)
			return n_x - MAP_SIZE;
		else if (n_x < 0)
			return n_x + MAP_SIZE;
		else
			return n_x;
	}()};
	const auto new_y{[&] {
		if (n_y > MAP_SIZE)
			return n_y - MAP_SIZE;
		else if (n_y < 0)
			return n_y + MAP_SIZE;
		else
			return n_y;
	}()};

	return _tiles.at(COORD2VECPOS(new_x, new_y));
}

auto Sorcery::Map::at(unsigned int x, unsigned int y) -> Tile & {

	return _tiles.at(COORD2VECPOS(x, y));
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

		// A blank level
		_tiles.clear();
		_tiles.reserve(MAP_SIZE * MAP_SIZE);
		for (auto x = 0u; x < MAP_SIZE; x++) {
			for (auto y = 0u; y < MAP_SIZE; y++) {
				Tile tile(Point{x, y});
				tile.set_walkable();
				_tiles.emplace_back(tile);
			}
		}

		break;
	case MapType::START: {
		_tiles.clear();
		_tiles.reserve(MAP_SIZE * MAP_SIZE);
		for (auto x = 0u; x < MAP_SIZE; x++) {
			for (auto y = 0u; y < MAP_SIZE; y++) {
				Tile tile(Point{x, y});
				tile.set_walkable();
				_tiles.emplace_back(tile);
			}
		}

		// Set Corner Tiles
		_tiles.at(COORD2VECPOS(0, 0)).set_walls(false, true, false, true);
		_tiles.at(COORD2VECPOS(0, MAP_SIZE - 1))
			.set_walls(true, false, false, true);
		_tiles.at(COORD2VECPOS(MAP_SIZE - 1, 0))
			.set_walls(false, true, true, false);
		_tiles.at(COORD2VECPOS(MAP_SIZE - 1, MAP_SIZE - 1))
			.set_walls(true, false, true, false);

		// Set Outside Walls
		for (auto y = 1u; y < MAP_SIZE - 1; y++) {
			auto &tile_left{_tiles.at(COORD2VECPOS(0, y))};
			tile_left.set_walls(false, false, false, true);

			auto &tile_right{_tiles.at(COORD2VECPOS(MAP_SIZE - 1, y))};
			tile_right.set_walls(false, false, true, false);
		}

		for (auto x = 1u; x < MAP_SIZE - 1; x++) {
			auto &tile_top{_tiles.at(COORD2VECPOS(x, 0))};
			tile_top.set_walls(false, true, false, false);

			auto &tile_bottom{_tiles.at(COORD2VECPOS(x, MAP_SIZE - 1))};
			tile_bottom.set_walls(true, false, false, false);
		}

		for (auto y = 0u; y < 20u; y++) {
			for (auto x = 0u; x < 20u; x++) {
				auto &tile{_tiles.at((x * MAP_SIZE) + y)};
				tile.set_gfx(1);
			}
		}

		_tiles.at(0).set_explored();
		_tiles.at(0).features.at(TileFeature::STAIRS_UP) = true;

		// Now do some random features purely for testing
		for (auto count = 0; count < 20; count++) {

			auto i = rand() % 400;
			_tiles.at(i).set_explored();
			_tiles.at(i).features.at(TileFeature::SPINNER) = true;

			i = rand() % 400;
			_tiles.at(i).features.at(TileFeature::PIT) = true;

			i = rand() % 400;
			_tiles.at(i).features.at(TileFeature::TELEPORT) = true;

			i = rand() % 400;
			_tiles.at(i).features.at(TileFeature::POOL) = true;

			i = rand() % 400;
			_tiles.at(i).features.at(TileFeature::FOUNTAIN) = true;

			i = rand() % 400;
			_tiles.at(i).features.at(TileFeature::MESSAGE) = true;

			i = rand() % 400;
			_tiles.at(i).features.at(TileFeature::MOVEMENT_NORTH) = true;

			i = rand() % 400;
			_tiles.at(i).features.at(TileFeature::MOVEMENT_SOUTH) = true;

			i = rand() % 400;
			_tiles.at(i).features.at(TileFeature::MOVEMENT_EAST) = true;

			i = rand() % 400;
			_tiles.at(i).features.at(TileFeature::MOVEMENT_WEST) = true;
		}

		for (auto count = 0; count < 5; count++) {
			auto i = rand() % 400;
			_tiles.at(i).features.at(TileFeature::STAIRS_UP) = true;

			i = rand() % 400;
			_tiles.at(i).features.at(TileFeature::STAIRS_DOWN) = true;
		}

		for (auto count = 0; count < 50; count++) {
			auto i = rand() % 400;
			if ((_tiles.at(i).count_feature() == 0) &&
				(_tiles.at(i).count_property() == 0))
				_tiles.at(i).properties.at(TileProperty::ROCK) = true;

			i = rand() % 400;
			if ((_tiles.at(i).count_feature() == 0) &&
				(_tiles.at(i).count_property() == 0))
				_tiles.at(i).properties.at(TileProperty::DARKNESS) = true;
		}

	} break;
	default:
		_reset_level();
		break;
	}
}
