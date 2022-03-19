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

#include "level.hpp"

// Default Constructor
Sorcery::Level::Level() {}

// Standard Constructor
Sorcery::Level::Level(const MapType type, const std::string dungeon,
	const int depth, const Coordinate bottom_left, const Size size)
	: _type{type}, _dungeon{dungeon}, _depth{depth},
	  _bottom_left{bottom_left}, _size{size} {

	_tiles.clear();
}

// Copy Constructors
Sorcery::Level::Level(const Level &other)
	: _type{other._type}, _dungeon{other._dungeon}, _depth{_depth},
	  _bottom_left{_bottom_left}, _size{other._size} {

	_tiles = other._tiles;
}

auto Sorcery::Level::operator=(const Level &other) -> Level & {

	_type = other._type;
	_dungeon = other._dungeon;
	_depth = other._depth;
	_bottom_left = other._bottom_left;
	_size = other._size;
	_tiles = other._tiles;

	return *this;
}

auto Sorcery::Level::load(const Json::Value row_data) -> bool {

	for (auto j = 0u; j < row_data.size(); j++) {

		// Get the top level data items
		auto tile_data{row_data[j]["tdata"]};
		const auto start{static_cast<int>(row_data[j]["start"].asInt())};
		const auto current_y{static_cast<int>(row_data[j]["y"].asInt())};
		const auto absolute_x{static_cast<int>(_bottom_left.x + start)};
		auto x{0};
		auto y{current_y};

		// First pass through - build the tiles needed
		for (auto i = 0u; i < tile_data.size(); i++) {

			// For each cell
			x = absolute_x + i;
			auto tile{tile_data[i]};

			// Get the bottom and right walls and cell properties
			auto south_wall{[&] {
				if (tile.isMember("b"))
					return static_cast<unsigned int>(tile["b"].asUInt());
				else
					return 0u;
			}()};
			auto darkness{[&] {
				if (tile.isMember("d"))
					return static_cast<std::string>(tile["d"].asString()) ==
						   "1";
				else
					return false;
			}()};
			auto marker{[&] {
				if (tile.isMember("m"))
					return static_cast<unsigned int>(tile["m"].asUInt());
				else
					return 0u;
			}()};
			auto east_wall{[&] {
				if (tile.isMember("r"))
					return static_cast<unsigned int>(tile["r"].asUInt());
				else
					return 0u;
			}()};
			auto terrain{[&] {
				if (tile.isMember("t"))
					return static_cast<unsigned int>(tile["t"].asUInt());
				else
					return 0u;
			}()};

			_add_tile(Coordinate{x, y}, south_wall, east_wall, darkness, marker,
				terrain);
		}
	}

	for (auto j = 0u; j < row_data.size(); j++) {

		// Get the top level data items
		const auto start{static_cast<int>(row_data[j]["start"].asInt())};
		const auto absolute_x{static_cast<int>(_bottom_left.x + start)};
		const auto current_y{static_cast<int>(row_data[j]["y"].asInt())};
		auto tile_data{row_data[j]["tdata"]};
		auto x{0};
		auto y{current_y};

		// Second pass through - build the other edges of the tiles
		for (auto i = 0u; i < tile_data.size(); i++) {

			// For each cell
			x = absolute_x + i;
			auto tile{tile_data[i]};

			// Get the bottom and right walls but remembering these are the
			// top and left walls of adjacent tiles - see
			// https://docs.gridcartographer.com/ref/tile_data_models for
			// details
			auto north_wall{[&] {
				if (tile.isMember("b"))
					return static_cast<unsigned int>(tile["b"].asUInt());
				else
					return 0u;
			}()};
			auto west_wall{[&] {
				if (tile.isMember("r"))
					return static_cast<unsigned int>(tile["r"].asUInt());
				else
					return 0u;
			}()};
			auto new_x{x - 1};
			auto new_y{y - 1};

			_update_tile(Coordinate{new_x, new_y}, north_wall, west_wall);
		}
	}

	return true;
}

auto Sorcery::Level::name() const -> std::string {

	return _depth < 0 ? fmt::format("{} B{}F", _dungeon, std::abs(_depth))
					  : fmt::format("{} {}F", _dungeon, std::abs(_depth));
}

// Since Grid Cartographer only defines s/e walls in our format, we do
// an add and then an update from the adjacent tiles on another pass
auto Sorcery::Level::_add_tile(const Coordinate location,
	const unsigned int south_wall, const unsigned int east_wall,
	const bool darkness, const unsigned int marker, const unsigned int terrain)
	-> void {

	auto south_edge{_convert_edge_se(south_wall)};
	auto east_edge{_convert_edge_se(east_wall)};

	Tile_ tile{location, std::nullopt, south_edge, east_edge, std::nullopt};
	if (darkness)
		tile.set(TileProperty::DARKNESS);

	_tiles[location] = tile;
}

auto Sorcery::Level::_update_tile(const Coordinate location,
	const unsigned int north_wall, const unsigned int west_wall) -> void {

	auto north_edge{_convert_edge_nw(north_wall)};
	auto west_edge{_convert_edge_nw(west_wall)};

	auto &tile{_tiles.at(location)};

	tile.set(MapDirection::NORTH, north_edge);
	tile.set(MapDirection::WEST, west_edge);
}

// Due to the way GC defines levels, we need to handle different edges
// differently so this is the inner function
auto Sorcery::Level::_convert_edge(const unsigned int wall) const
	-> std::optional<TileEdge> {

	std::optional<TileEdge> edge{std::nullopt};
	switch (wall) { // NOLINT(clang-diagnostic-switch)
	case 0:
		edge = TileEdge::NONE;
		break;
	case 1:
		edge = TileEdge::WALL;
		break;
	case 2:
		[[fallthrough]];
	case 12:
		[[fallthrough]];
	case 33:
		edge = TileEdge::UNLOCKED_DOOR;
		break;
	case 3:
		edge = TileEdge::LOCKED_DOOR;
		break;
	case 4:
		edge = TileEdge::HIDDEN_DOOR;
		break;
	case 13:
		edge = TileEdge::SECRET_WALL;
		break;
	case 29:
		edge = TileEdge::SECRET_DOOR;
		break;
	default:
		break;
	}

	return edge;
}

// From the S/E perspectives, the one way doors etc are walls
auto Sorcery::Level::_convert_edge_se(const unsigned int wall) const
	-> TileEdge {

	std::optional<TileEdge> standard_edge{_convert_edge(wall)};
	if (standard_edge.has_value()) {
		return standard_edge.value();
	} else {
		TileEdge edge{TileEdge::NONE};
		switch (wall) { // NOLINT(clang-diagnostic-switch)
		case 5:
			edge = TileEdge::WALL;
			break;
		case 6:
			edge = TileEdge::WALL;
			break;
		case 7:
			edge = TileEdge::WALL;
			break;
		case 8:
			edge = TileEdge::ONE_WAY_DOOR;
			break;
		case 9:
			edge = TileEdge::HIDDEN_DOOR;
			break;
		case 10:
			edge = TileEdge::ONE_WAY_WALL;
			break;
		default:
			break;
		}

		return edge;
	}
}

// From the N/W perspectives, the one way doors etc are walls
auto Sorcery::Level::_convert_edge_nw(const unsigned int wall) const
	-> TileEdge {

	std::optional<TileEdge> standard_edge{_convert_edge(wall)};
	if (standard_edge.has_value()) {
		return standard_edge.value();
	} else {
		TileEdge edge{TileEdge::NONE};
		switch (wall) { // NOLINT(clang-diagnostic-switch)
		case 5:
			edge = TileEdge::ONE_WAY_DOOR;
			break;
		case 6:
			edge = TileEdge::HIDDEN_DOOR;
			break;
		case 7:
			edge = TileEdge::ONE_WAY_WALL;
			break;
		case 8:
			edge = TileEdge::WALL;
			break;
		case 9:
			edge = TileEdge::WALL;
			break;
		case 10:
			edge = TileEdge::WALL;
			break;
		default:
			break;
		}

		return edge;
	}
}
