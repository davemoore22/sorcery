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
Sorcery::Level::Level() {

	_create();
}

// Standard Constructor
Sorcery::Level::Level(const MapType type, const std::string dungeon,
	const int depth, const Coordinate bottom_left, const Size size)
	: _type{type}, _dungeon{dungeon}, _depth{depth},
	  _bottom_left{bottom_left}, _size{size} {

	_create();
}

// Copy Constructors
Sorcery::Level::Level(const Level &other)
	: _type{other._type}, _dungeon{other._dungeon}, _depth{other._depth},
	  _bottom_left{other._bottom_left}, _size{other._size} {

	_tiles = other._tiles;
	_notes = other._notes;
}

auto Sorcery::Level::operator=(const Level &other) -> Level & {

	_type = other._type;
	_dungeon = other._dungeon;
	_depth = other._depth;
	_bottom_left = other._bottom_left;
	_size = other._size;
	_tiles = other._tiles;
	_notes = other._notes;

	return *this;
}

auto Sorcery::Level::operator[](Coordinate loc) -> Tile & {

	return _tiles.at(loc);
}

auto Sorcery::Level::operator()(Coordinate loc) -> TileNote {

	if (_notes.contains(loc))
		return _notes.at(loc);
	else
		return TileNote{};
}

auto Sorcery::Level::reset() -> void {

	_create();
}

auto Sorcery::Level::depth() const -> int {

	return _depth;
}

auto Sorcery::Level::bottom_left() const -> Coordinate {

	return _bottom_left;
}

auto Sorcery::Level::top_right() const -> Coordinate {

	return Coordinate{_bottom_left.x + static_cast<int>(_size.w),
		_bottom_left.y + static_cast<int>(_size.h)};
}

auto Sorcery::Level::wrap_bottom_left() const -> Coordinate {

	return Coordinate{_bottom_left.x + 1, _bottom_left.y};
}

auto Sorcery::Level::wrap_size() const -> Size {

	return Size{_size.w - 1, _size.h - 1};
}

auto Sorcery::Level::wrap_top_right() const -> Coordinate {

	// 2 here instead of 1 to fix issue with GC data starting at -1,0 and thus
	// having an extra square on top (its 21x21 squares)
	return Coordinate{
		_bottom_left.x + _size.w - 1, _bottom_left.y + _size.h - 2};
}

auto Sorcery::Level::size() const -> Size {

	return _size;
}

auto Sorcery::Level::type() const -> MapType {

	return _type;
}

auto Sorcery::Level::in(const Coordinate loc) const -> bool {

	return (loc.x >= _bottom_left.x) &&
		   (loc.x <= _bottom_left.x + static_cast<int>(_size.w)) &&
		   (loc.y >= _bottom_left.y) &&
		   (loc.y <= _bottom_left.y + static_cast<int>(_size.h));
}

auto Sorcery::Level::load(
	const Json::Value row_data, const Json::Value note_data) -> bool {

	_create();
	_load_first_pass(row_data);
	_load_second_pass(row_data);
	_load_third_pass();
	_load_notes(note_data);

	return true;
}

auto Sorcery::Level::set(const Level *other) -> void {

	_type = other->_type;
	_dungeon = other->_dungeon;
	_depth = other->_depth;
	_bottom_left = other->_bottom_left;
	_size = other->_size;
	_tiles = other->_tiles;
	_notes = other->_notes;
}

auto Sorcery::Level::note_at(const Coordinate loc) -> TileNote {

	if (_notes.contains(loc))
		return _notes.at(loc);
	else {
		return TileNote{};
	}
}
auto Sorcery::Level::note_at(const int x, const int y) -> TileNote {

	if (_notes.contains(Coordinate{x, y}))
		return _notes.at(Coordinate{x, y});
	else {
		return TileNote{};
	}
}

auto Sorcery::Level::at(const Coordinate loc) -> Tile & {

	return _tiles.at(loc);
}

auto Sorcery::Level::at(const Coordinate loc, const MapDirection direction,
	const int x, const int z) -> Tile & {

	// Needs to be done seperately since levels have an extra row/column, and we
	// must also remember that N/E is actually y/x
	Coordinate dest{loc};
	switch (direction) {
	case MapDirection::NORTH: {
		dest.x = loc.x + x;
		dest.y = loc.y + z;
	} break;
	case MapDirection::SOUTH: {
		dest.x = loc.x - x;
		dest.y = loc.y - z;
	} break;
	case MapDirection::EAST: {
		dest.x = loc.x + z;
		dest.y = loc.y - x;
	} break;
	case MapDirection::WEST: {
		dest.x = loc.x - z;
		dest.y = loc.y + x;
	} break;
	default:
		return _tiles.at(loc);
		break;
	}

	auto wrapped_x{[&] {
		if (dest.x < wrap_bottom_left().x)
			return dest.x + static_cast<int>(wrap_size().w);
		else if (dest.x > wrap_top_right().x)
			return dest.x - static_cast<int>(wrap_size().w);
		return dest.x;
	}()};
	auto wrapped_y{[&] {
		if (dest.y < wrap_bottom_left().y)
			return dest.y + static_cast<int>(wrap_size().h);
		else if (dest.y > wrap_top_right().y)
			return dest.y - static_cast<int>(wrap_size().h);
		return dest.y;
	}()};
	return _tiles.at(Coordinate{wrapped_x, wrapped_y});
}

auto Sorcery::Level::at(const int x, const int y) -> Tile & {

	return _tiles.at(Coordinate{x, y});
}

auto Sorcery::Level::get_delta_x(const int x, const int delta) const -> int {

	auto new_x{x + delta};
	if (new_x < wrap_bottom_left().x)
		return new_x + static_cast<int>(wrap_size().w);
	else if (new_x > wrap_top_right().x)
		return new_x - static_cast<int>(wrap_size().w);
	else
		return new_x;
}

auto Sorcery::Level::get_delta_y(const int y, const int delta) const -> int {

	auto new_y{y + delta};
	if (new_y < wrap_bottom_left().y)
		return new_y + static_cast<int>(wrap_size().h);
	else if (new_y > wrap_top_right().y)
		return new_y - static_cast<int>(wrap_size().h);
	else
		return new_y;
}

auto Sorcery::Level::_create() -> void {

	_tiles.clear();

	// Create the blank tiles because GC export data doesn't always include
	// empty tiles to save space in the export
	for (auto y = _bottom_left.y;
		 y <= _bottom_left.y + static_cast<int>(_size.h); y++) {
		for (auto x = _bottom_left.x;
			 x <= _bottom_left.x + static_cast<int>(_size.w); x++) {
			_add_tile(Coordinate{x, y});
		}
	}
}

auto Sorcery::Level::_load_notes(const Json::Value note_data) -> bool {

	for (auto j = 0u; j < note_data.size(); j++) {

		const auto x{static_cast<int>(note_data[j]["x"].asInt())};
		const auto y{static_cast<int>(note_data[j]["y"].asInt())};
		const auto text{note_data[j]["__data"].asString()};

		TileNote note{x, y, text};
		_notes[Coordinate{x, y}] = note;
	}
}

auto Sorcery::Level::_load_first_pass(const Json::Value row_data) -> bool {

	for (auto j = 0u; j < row_data.size(); j++) {

		// Get the top level data items
		auto tile_data{row_data[j]["tdata"]};
		const auto start{static_cast<int>(row_data[j]["start"].asInt())};
		const auto absolute_x{static_cast<int>(_bottom_left.x + start)};
		const auto absolute_y{static_cast<int>(_bottom_left.y)};
		const auto current_y{static_cast<int>(row_data[j]["y"].asInt())};
		auto x{0};
		auto y{current_y + absolute_y};

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

			_update_tile(Coordinate{x, y}, south_wall, east_wall, darkness,
				marker, terrain);
		}
	}

	return true;
}

auto Sorcery::Level::_load_second_pass(const Json::Value row_data) -> bool {

	return true;

	for (auto j = 0u; j < row_data.size(); j++) {

		// Get the top level data items
		const auto start{static_cast<int>(row_data[j]["start"].asInt())};
		const auto absolute_x{static_cast<int>(_bottom_left.x + start)};
		const auto absolute_y{static_cast<int>(_bottom_left.y)};
		const auto current_y{static_cast<int>(row_data[j]["y"].asInt())};
		auto tile_data{row_data[j]["tdata"]};
		auto x{0};
		auto y{current_y + absolute_y};

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
			auto new_x{x};
			auto new_y{y};

			_update_tile(
				Coordinate{new_x + 1, new_y + 1}, north_wall, west_wall);
		}
	}

	return true;
}

// Check for single normal walls and double them as we are using dual walls
// so for each side, check if its a normal wall/door and if the other side
// is empty then give the other side the same wall/door
auto Sorcery::Level::_load_third_pass() -> bool {

	// Use the Wrapping "View" to guarantee tiles exist
	for (auto y = wrap_bottom_left().y; y <= wrap_top_right().y; y++)
		for (auto x = wrap_bottom_left().x; x <= wrap_top_right().x; x++)
			_set_other_edges(Coordinate{x, y});
}

auto Sorcery::Level::name() const -> std::string {

	return _depth < 0 ? fmt::format("{} B{}F", _dungeon, std::abs(_depth))
					  : fmt::format("{} {}F", _dungeon, std::abs(_depth));
}

auto Sorcery::Level::_add_tile(const Coordinate location) -> void {

	Tile tile{location};
	_tiles[location] = tile;
}

// Since Grid Cartographer only defines s/e walls in our format, we do two
// updates, first with the tile in question, and then from the adjacent tile
// on another pass
auto Sorcery::Level::_update_tile(const Coordinate location,
	const unsigned int south_wall, const unsigned int east_wall,
	const bool darkness, const unsigned int marker, const unsigned int terrain)
	-> void {

	auto south_edge{_convert_edge_se(south_wall)};
	auto east_edge{_convert_edge_se(east_wall)};

	auto &tile{_tiles.at(location)};
	tile.set(MapDirection::SOUTH, south_edge);
	tile.set(MapDirection::EAST, east_edge);
	if (darkness)
		tile.set(TileProperty::DARKNESS);

	// https://docs.gridcartographer.com/ref/table/marker
	if (marker == 1)
		tile.set(TileFeature::STAIRS_UP);
	else if (marker == 2)
		tile.set(TileFeature::STAIRS_DOWN);
	else if (marker == 25)
		tile.set(TileFeature::MESSAGE);
	else if (marker == 108)
		tile.set(TileFeature::MESSAGE);
	else if (marker == 26)
		tile.set(TileFeature::STAIRS_UP);
	else if (marker == 27)
		tile.set(TileFeature::STAIRS_DOWN);

	_tiles[location] = tile;
}

auto Sorcery::Level::_update_tile(const Coordinate location,
	const unsigned int north_wall, const unsigned int west_wall) -> void {

	auto north_edge{_convert_edge_nw(north_wall)};
	auto west_edge{_convert_edge_nw(west_wall)};

	auto &tile{_tiles.at(location)};

	if (!tile.has(MapDirection::NORTH))
		tile.set(MapDirection::NORTH, north_edge);
	if (!tile.has(MapDirection::WEST))
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

auto Sorcery::Level::_set_other_edges(const Coordinate location) -> void {

	auto &tile{_tiles.at(location)};
	auto north_edge{tile.wall(MapDirection::NORTH)};
	if (north_edge == TileEdge::NONE) {

		// Check north adjacent wall (i.e. south wall of above tile)
		auto adj_north{
			_tiles.at(Coordinate{location.x, get_delta_y(location.y, 1)})};
		auto adj_north_edge{adj_north.wall(MapDirection::SOUTH)};

		switch (adj_north_edge) {
		case TileEdge::UNLOCKED_DOOR:
			[[fallthrough]];
		case TileEdge::WALL:
			[[fallthrough]];
		case TileEdge::SECRET_DOOR:
			[[fallthrough]];
		case TileEdge::LOCKED_DOOR:
			tile.set(MapDirection::NORTH, adj_north_edge);
			break;
		default:
			break;
		}
	}

	auto south_edge{tile.wall(MapDirection::SOUTH)};
	if (south_edge == TileEdge::NONE) {

		// Check south adjacent wall (i.e. borth wall of below tile)
		auto adj_south{
			_tiles.at(Coordinate{location.x, get_delta_y(location.y, -1)})};
		auto adj_south_edge{adj_south.wall(MapDirection::NORTH)};

		switch (adj_south_edge) {
		case TileEdge::UNLOCKED_DOOR:
			[[fallthrough]];
		case TileEdge::WALL:
			[[fallthrough]];
		case TileEdge::SECRET_DOOR:
			[[fallthrough]];
		case TileEdge::LOCKED_DOOR:
			tile.set(MapDirection::SOUTH, adj_south_edge);
			break;
		default:
			break;
		}
	}

	auto west_edge{tile.wall(MapDirection::WEST)};
	if (west_edge == TileEdge::NONE) {

		// Check west adjacent wall (i.e. east wall of left tile)
		auto adj_west{
			_tiles.at(Coordinate{get_delta_x(location.x, -1), location.y})};
		auto adj_west_edge{adj_west.wall(MapDirection::EAST)};

		switch (adj_west_edge) {
		case TileEdge::UNLOCKED_DOOR:
			[[fallthrough]];
		case TileEdge::WALL:
			[[fallthrough]];
		case TileEdge::SECRET_DOOR:
			[[fallthrough]];
		case TileEdge::LOCKED_DOOR:
			tile.set(MapDirection::WEST, adj_west_edge);
			break;
		default:
			break;
		}
	}

	auto east_edge{tile.wall(MapDirection::EAST)};
	if (east_edge == TileEdge::NONE) {

		// Check west adjacent wall (i.e. east wall of left tile)
		auto adj_east{
			_tiles.at(Coordinate{get_delta_x(location.x, 1), location.y})};
		auto adj_east_edge{adj_east.wall(MapDirection::WEST)};

		switch (adj_east_edge) {
		case TileEdge::UNLOCKED_DOOR:
			[[fallthrough]];
		case TileEdge::WALL:
			[[fallthrough]];
		case TileEdge::SECRET_DOOR:
			[[fallthrough]];
		case TileEdge::LOCKED_DOOR:
			tile.set(MapDirection::EAST, adj_east_edge);
			break;
		default:
			break;
		}
	}
}