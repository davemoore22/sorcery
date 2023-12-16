// Copyright (C) 2023 Dave Moore
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

#include "types/level.hpp"

// Default Constructor
Sorcery::Level::Level() {

	_size.w = 20;
	_size.h = 20;
	_create();
}

// Standard Constructor
Sorcery::Level::Level(
	const MapType type, const std::string dungeon, const int depth, const Coordinate bottom_left, const Size size)
	: _type{type}, _dungeon{dungeon}, _depth{depth}, _bottom_left{bottom_left}, _size{size} {

	_create();
}

// Copy Constructors
Sorcery::Level::Level(const Level &other)
	: _type{other._type}, _dungeon{other._dungeon}, _depth{other._depth}, _bottom_left{other._bottom_left},
	  _size{other._size} {

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

auto Sorcery::Level::operator[](Coordinate loc) -> Tile & {

	return _tiles.at(loc);
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

	return Coordinate{_bottom_left.x + static_cast<int>(_size.w), _bottom_left.y + static_cast<int>(_size.h)};
}

auto Sorcery::Level::wrap_bottom_left() const -> Coordinate {

	return Coordinate{_bottom_left.x + 1, _bottom_left.y};
}

auto Sorcery::Level::wrap_size() const -> Size {

	return Size{_size.w - 1, _size.h - 1};
}

auto Sorcery::Level::wrap_top_right() const -> Coordinate {

	// 2 here instead of 1 to fix issue with GC data starting at -1,0 and thus having an extra square on top (its 21x21
	// squares)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
	return Coordinate{_bottom_left.x + _size.w - 1, _bottom_left.y + _size.h - 2};
#pragma GCC diagnostic pop
}

auto Sorcery::Level::size() const -> Size {

	return _size;
}

auto Sorcery::Level::type() const -> MapType {

	return _type;
}

auto Sorcery::Level::in(const Coordinate loc) const -> bool {

	return (loc.x >= _bottom_left.x) && (loc.x <= _bottom_left.x + static_cast<int>(_size.w)) &&
		   (loc.y >= _bottom_left.y) && (loc.y <= _bottom_left.y + static_cast<int>(_size.h));
}

auto Sorcery::Level::load(const Json::Value row_data, const Json::Value note_data) -> bool {

	_create();
	_load_simple_walls(row_data);
	_fill_in_simple_walls();
	_set_complicated_walls(row_data);
	_load_markers(row_data);
	_load_metadata(note_data);

	return true;
}

auto Sorcery::Level::set(const Level *other) -> void {

	_type = other->_type;
	_dungeon = other->_dungeon;
	_depth = other->_depth;
	_bottom_left = other->_bottom_left;
	_size = other->_size;
	_tiles = other->_tiles;
}

auto Sorcery::Level::at(const Coordinate loc) -> Tile & {

	return _tiles.at(loc);
}

auto Sorcery::Level::at(const Coordinate loc, const MapDirection direction, const int x, const int z) -> Tile & {

	// Needs to be done seperately since levels have an extra row/column, and we must also remember that N/E is actually
	// y/x
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

	// Create the blank tiles because GC export data doesn't always include empty tiles to save space in the export
	for (auto y = _bottom_left.y; y <= _bottom_left.y + static_cast<int>(_size.h); y++) {
		for (auto x = _bottom_left.x; x <= _bottom_left.x + static_cast<int>(_size.w); x++) {
			_add_tile(Coordinate{x, y});
		}
	}

	_add_event_mappings();
}

auto Sorcery::Level::_add_event_mappings() -> void {

	using enum Enums::Map::Event;

	_event_mappings.clear();

	_event_mappings["AREA_OUT_OF_BOUNDS"] = AREA_OF_OUT_BOUNDS;

	_event_mappings["MAN_TELEPORT_CASTLE"] = MAN_TELEPORT_CASTLE;
	_event_mappings["SILVER_KEY"] = SILVER_KEY;
	_event_mappings["BRONZE_KEY"] = BRONZE_KEY;
	_event_mappings["MURPHYS_GHOSTS"] = MURPHYS_GHOSTS;
	_event_mappings["BEAR_STATUE"] = BEAR_STATUE;
	_event_mappings["FROG_STATUE"] = FROG_STATUE;
	_event_mappings["GOLD_KEY"] = GOLD_KEY;
	_event_mappings["NEED_SILVER_KEY"] = NEED_SILVER_KEY;
	_event_mappings["NEED_BRONZE_KEY"] = NEED_BRONZE_KEY;
	_event_mappings["NEED_BEAR_STATUE"] = NEED_BEAR_STATUE;

	_event_mappings["NEED_FROG_STATUE"] = NEED_FROG_STATUE;
	_event_mappings["PLACARD_PIT_1"] = PLACARD_PIT_1;
	_event_mappings["PLACARD_PIT_2"] = PLACARD_PIT_2;
	_event_mappings["PLACARD_PIT_3"] = PLACARD_PIT_3;
	_event_mappings["TURN_AROUND"] = TURN_AROUND;
	_event_mappings["TURN_LEFT"] = TURN_LEFT;
	_event_mappings["TURN_RIGHT"] = TURN_RIGHT;
	_event_mappings["NEED_BEAR_STATUE_2"] = NEED_BEAR_STATUE_2;
	_event_mappings["TESTING_GROUNDS"] = TESTING_GROUNDS;
	_event_mappings["ALARM_BELLS"] = ALARM_BELLS;

	_event_mappings["TREASURE_REPOSITORY"] = TREASURE_REPOSITORY;
	_event_mappings["MONSTER_ALLOCATION_CENTRE"] = MONSTER_ALLOCATION_CENTRE;
	_event_mappings["LARGE_DESK"] = LARGE_DESK;
	_event_mappings["TREBOR_VOICE"] = TREBOR_VOICE;
	_event_mappings["SERVICE_ELEVATOR"] = SERVICE_ELEVATOR;
	_event_mappings["WERDNA_BOAST"] = WERDNA_BOAST;
	_event_mappings["TURN_BACK"] = TURN_BACK;
	_event_mappings["WERDNA_SIGN"] = WERDNA_SIGN;
	_event_mappings["THREE_HUMANOIDS"] = THREE_HUMANOIDS;
	_event_mappings["GETTING_WARM_1"] = GETTING_WARM_1;

	_event_mappings["GETTING_WARM_2"] = GETTING_WARM_2;
	_event_mappings["GETTING_WARM_3"] = GETTING_WARM_3;
	_event_mappings["FIRE_DRAGONS_COMBAT"] = FIRE_DRAGONS_COMBAT;
	_event_mappings["DEADLY_RING_COMBAT"] = DEADLY_RING_COMBAT;
	_event_mappings["WERDNA_COMBAT"] = WERDNA_COMBAT;
	_event_mappings["GUARANTEED_COMBAT"] = GUARANTEED_COMBAT;
	_event_mappings["NEED_BLUE_RIBBON"] = NEED_BLUE_RIBBON;
}

auto Sorcery::Level::_load_metadata(const Json::Value note_data) -> bool {

	for (auto j = 0u; j < note_data.size(); j++) {

		const auto x{static_cast<int>(note_data[j]["x"].asInt())};
		const auto y{static_cast<int>(note_data[j]["y"].asInt())};
		const auto text{note_data[j]["__data"].asString()};
		const auto found_pos{text.find("METADATA")};
		if (found_pos != std::string::npos) {

			const auto metadata{text.substr(found_pos)};
			if (metadata.starts_with("METADATA")) {

				auto data{SPLIT(metadata)};
				if ((data.at(1) == "TELEPORT") && (data.at(2) == "TO")) {
					Teleport teleport{std::stoi(data.at(3)), Coordinate{std::stoi(data.at(4)), std::stoi(data.at(5))}};
					_tiles.at(Coordinate{x, y}).set_teleport(teleport);
				} else if ((data.at(1) == "CHUTE") && (data.at(2) == "TO")) {
					Teleport teleport{std::stoi(data.at(3)), Coordinate{std::stoi(data.at(4)), std::stoi(data.at(5))}};
					_tiles.at(Coordinate{x, y}).set_teleport(teleport);
				} else if ((data.at(1) == "STAIRS") && (data.at(2) == "TO")) {
					Teleport stairs{std::stoi(data.at(3)), Coordinate{std::stoi(data.at(4)), std::stoi(data.at(5))}};
					_tiles.at(Coordinate{x, y}).set_stairs(stairs);
				} else if (data.at(1) == "ELEVATOR") {
					const auto up{data.at(2) == "UP"};
					const auto down{data.at(3) == "DOWN"};
					const auto up_loc{up ? Coordinate{x, y} : Coordinate{0, 0}};
					const auto down_loc{down ? Coordinate{x, y} : Coordinate{0, 0}};
					Elevator elevator{up, up_loc, down, down_loc, std::stoi(data.at(4)), std::stoi(data.at(5))};
					_tiles.at(Coordinate{x, y}).set_elevator(elevator);
					if (up)
						_tiles.at(Coordinate{x, y}).set(TileFeature::ELEVATOR_UP);
					if (down)
						_tiles.at(Coordinate{x, y}).set(TileFeature::ELEVATOR_DOWN);
				} else if (data.at(1) == "EVENT") {
					const auto &what{data.at(2)};
					const auto event{_map_event_types(what)};
					_tiles.at(Coordinate{x, y}).set(event);
				}
			}
		}
	}

	return true;
}

auto Sorcery::Level::_map_event_types(const std::string &string) const -> std::optional<MapEvent> {

	if (_event_mappings.contains(string))
		return _event_mappings.at(string);

	return std::nullopt;
}

auto Sorcery::Level::_load_markers(const Json::Value row_data) -> bool {

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

			auto darkness{[&] {
				if (tile.isMember("d"))
					return static_cast<std::string>(tile["d"].asString()) == "1";
				else
					return false;
			}()};
			auto marker{[&] {
				if (tile.isMember("m"))
					return static_cast<unsigned int>(tile["m"].asUInt());
				else
					return 0u;
			}()};
			auto terrain{[&] {
				if (tile.isMember("t"))
					return static_cast<unsigned int>(tile["t"].asUInt());
				else
					return 0u;
			}()};

			_update_tile_markers(Coordinate{x, y}, darkness, marker, terrain);
		}
	}

	return true;
}

auto Sorcery::Level::_set_complicated_walls(const Json::Value row_data) -> bool {

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
			auto east_wall{[&] {
				if (tile.isMember("r"))
					return static_cast<unsigned int>(tile["r"].asUInt());
				else
					return 0u;
			}()};

			_fill_in_complicated_walls(Coordinate{x, y}, south_wall, east_wall);
		}
	}

	return true;
}

auto Sorcery::Level::_load_simple_walls(const Json::Value row_data) -> bool {

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
			auto east_wall{[&] {
				if (tile.isMember("r"))
					return static_cast<unsigned int>(tile["r"].asUInt());
				else
					return 0u;
			}()};

			_update_tile_walls_simple(Coordinate{x, y}, south_wall, east_wall);
		}
	}

	return true;
}

// Since Grid Cartographer only defines s/e walls in our format, we do two updates, first with the tile in question, and
// then from the adjacent tile on another pass - but for now only update simple walls
auto Sorcery::Level::_update_tile_walls_simple(
	const Coordinate location, const unsigned int south_wall, const unsigned int east_wall) -> void {

	auto south_edge{_convert_edge_se(south_wall)};
	auto east_edge{_convert_edge_se(east_wall)};

	auto &tile{_tiles.at(location)};
	tile.set(MapDirection::SOUTH, south_edge.value());
	tile.set(MapDirection::EAST, east_edge.value());
}

// Check for single normal walls and double them as we are using dual walls so for each side, check if its a normal
// wall/door and if the other side is empty then give the other side the same wall/door
auto Sorcery::Level::_fill_in_simple_walls() -> bool {

	// Use the Wrapping "View" to guarantee tiles exist
	for (auto y = wrap_bottom_left().y; y <= wrap_top_right().y; y++)
		for (auto x = wrap_bottom_left().x; x <= wrap_top_right().x; x++)
			_set_other_simple_edges(Coordinate{x, y});

	return true;
}

auto Sorcery::Level::_set_other_simple_edges(const Coordinate location) -> void {

	auto &tile{_tiles.at(location)};
	auto north_edge{tile.wall(MapDirection::NORTH)};
	if (north_edge == TileEdge::NO_EDGE) {

		// Check north adjacent wall (i.e. south wall of above tile)
		auto adj_north{_tiles.at(Coordinate{location.x, get_delta_y(location.y, 1)})};
		auto adj_north_edge{adj_north.wall(MapDirection::SOUTH)};

		switch (adj_north_edge) {
		case TileEdge::UNLOCKED_DOOR:
			[[fallthrough]];
		case TileEdge::WALL:
			[[fallthrough]];
		case TileEdge::SECRET_DOOR: // TODO:: secret walls
			[[fallthrough]];
		case TileEdge::LOCKED_DOOR:
			tile.set(MapDirection::NORTH, adj_north_edge);
			break;
		default:
			break;
		}
	}

	auto south_edge{tile.wall(MapDirection::SOUTH)};
	if (south_edge == TileEdge::NO_EDGE) {

		// Check south adjacent wall (i.e. borth wall of below tile)
		auto adj_south{_tiles.at(Coordinate{location.x, get_delta_y(location.y, -1)})};
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
	if (west_edge == TileEdge::NO_EDGE) {

		// Check west adjacent wall (i.e. east wall of left tile)
		auto adj_west{_tiles.at(Coordinate{get_delta_x(location.x, -1), location.y})};
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
	if (east_edge == TileEdge::NO_EDGE) {

		// Check west adjacent wall (i.e. east wall of left tile)
		auto adj_east{_tiles.at(Coordinate{get_delta_x(location.x, 1), location.y})};
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

auto Sorcery::Level::name() const -> std::string {

	return _depth < 0 ? fmt::format("{} B{}F", _dungeon, std::abs(_depth))
					  : fmt::format("{} {}F", _dungeon, std::abs(_depth));
}

auto Sorcery::Level::_add_tile(const Coordinate location) -> void {

	Tile tile{location};
	_tiles[location] = tile;
}

auto Sorcery::Level::_update_tile_markers(const Coordinate location, const bool darkness, const unsigned int marker,
	[[maybe_unused]] const unsigned int terrain) -> void {

	// https://docs.gridcartographer.com/ref/table/marker
	auto &tile{_tiles.at(location)};

	if (darkness)
		tile.set(TileProperty::DARKNESS);
	switch (marker) {
	case 1:
		tile.set(TileFeature::STAIRS_UP);
		break;
	case 2:
		tile.set(TileFeature::STAIRS_DOWN);
		break;
	case 4:
		tile.set(TileFeature::TELEPORT_FROM);
		break;
	case 5:
		tile.set(TileFeature::TELEPORT_TO);
		break;
	case 7:
		tile.set(TileFeature::PIT);
		break;
	case 10:
		tile.set(TileFeature::PORTAL);
		break;
	case 11:
		tile.set(TileFeature::SPINNER);
		break;
	case 21:
		tile.set(TileFeature::ELEVATOR);
		break;
	case 25:
		tile.set(TileFeature::MESSAGE);
		break;
	case 26:
		tile.set(TileFeature::LADDER_UP);
		break;
	case 27:
		tile.set(TileFeature::LADDER_DOWN);
		break;
	case 50:
		tile.set(TileFeature::CHUTE);
		break;
	case 108:
		tile.set(TileFeature::NOTICE);
		break;

	default:
		break;
	}

	switch (terrain) {
	case 34:
		tile.set(TileProperty::ANTIMAGIC);
		break;
	case 35:
		tile.set(TileProperty::ROCK);
		tile.reset(TileProperty::WALKABLE);
		break;
	default:
		break;
	}
}

auto Sorcery::Level::elevator_at(const Coordinate loc) -> bool {

	const auto &tile{_tiles.at(loc)};
	return tile.has(TileFeature::ELEVATOR);
}

auto Sorcery::Level::stairs_at(const Coordinate loc) -> bool {

	const auto &tile{_tiles.at(loc)};
	return ((tile.has(TileFeature::LADDER_UP)) || (tile.has(TileFeature::LADDER_DOWN)) ||
			(tile.has(TileFeature::STAIRS_UP)) || (tile.has(TileFeature::STAIRS_DOWN)));
}

auto Sorcery::Level::_fill_in_complicated_walls(
	const Coordinate location, const unsigned int south_wall, const unsigned int east_wall) -> void {

	// OK, so this is a bit complicated due to GC only storing one set of walls per tile - we have to back fill in
	// complicated walls (walls that differ on each side)

	// Fortunately, the only walls that we need to worry about are:
	//
	// ONE_WAY_DOOR, HIDDEN_DOOR, ONE_WAY_WALL - in both direction
	//
	// We need to not only handle the walls in question but set the correspinding walls on adjacent tiles as needed

	// https: // docs.gridcartographer.com/ref/table/edge
	auto &tile{_tiles.at(location)};

	// Do South/North Walls
	auto &adj_south{_tiles.at(Coordinate{location.x, get_delta_y(location.y, -1)})};

	switch (south_wall) {
	case 5:
		// One-way door (exiting left or up)

		// Set south wall of current tile to wall
		// Set north wall of adjacent tile to One-way door
		tile.set(MapDirection::SOUTH, TileEdge::WALL);
		adj_south.set(MapDirection::NORTH, TileEdge::ONE_WAY_DOOR);
		break;
	case 6:
		// One-way hidden door (exiting left or up)

		// Set south wall of current tile to wall
		// Set north wall of adjacent tile to One-way hidden door
		tile.set(MapDirection::SOUTH, TileEdge::WALL);
		adj_south.set(MapDirection::NORTH, TileEdge::ONE_WAY_HIDDEN_DOOR);
		break;
	case 7:
		// One-way wall (exiting left or up)

		// Set south wall of current tile to wall
		// Set north wall of adjacent tile to One-way wall
		tile.set(MapDirection::SOUTH, TileEdge::WALL);
		adj_south.set(MapDirection::NORTH, TileEdge::ONE_WAY_WALL);
		break;

	case 8:
		// One-way door (exiting right or down)

		// Set south wall of current tile to One-way door
		// Set north wall of adjacent tile to wall
		tile.set(MapDirection::SOUTH, TileEdge::ONE_WAY_DOOR);
		adj_south.set(MapDirection::NORTH, TileEdge::WALL);
		break;
	case 9:
		// One-way hidden door (exiting right or down)

		// Set south wall of current tile to One-way hidden door
		// Set north wall of adjacent tile to wall
		tile.set(MapDirection::SOUTH, TileEdge::ONE_WAY_HIDDEN_DOOR);
		adj_south.set(MapDirection::NORTH, TileEdge::WALL);
		break;
	case 10:
		// One-way wall (exiting right or down)

		// Set south wall of current tile to One-way wall
		// Set north wall of adjacent tile to wall
		tile.set(MapDirection::SOUTH, TileEdge::ONE_WAY_WALL);
		adj_south.set(MapDirection::NORTH, TileEdge::WALL);
		break;
	default:
		break;
	}

	// Do East/West Walls
	auto &adj_east{_tiles.at(Coordinate{get_delta_x(location.x, 1), location.y})};

	switch (east_wall) {
	case 5:
		// One-way door (exiting left or up)

		// Set east wall of current tile to wall
		// Set west wall of adjacent tile to One-way door
		tile.set(MapDirection::EAST, TileEdge::WALL);
		adj_east.set(MapDirection::WEST, TileEdge::ONE_WAY_DOOR);
		break;
	case 6:
		// One-way hidden door (exiting left or up)

		// Set east wall of current tile to wall
		// Set west wall of adjacent tile to One-way hidden door
		tile.set(MapDirection::EAST, TileEdge::WALL);
		adj_east.set(MapDirection::WEST, TileEdge::ONE_WAY_HIDDEN_DOOR);
		break;
	case 7:
		// One-way wall (exiting left or up)

		// Set east wall of current tile to wall
		// Set west wall of adjacent tile to One-way wall
		tile.set(MapDirection::EAST, TileEdge::WALL);
		adj_east.set(MapDirection::WEST, TileEdge::ONE_WAY_WALL);
		break;

	case 8:
		// One-way door (exiting right or down)

		// Set east wall of current tile to One-way door
		// Set west wall of adjacent tile to wall
		tile.set(MapDirection::EAST, TileEdge::ONE_WAY_DOOR);
		adj_east.set(MapDirection::WEST, TileEdge::WALL);
		break;
	case 9:
		// One-way hidden door (exiting right or down)

		// Set east wall of current tile to One-way hidden door
		// Set west wall of adjacent tile to wall
		tile.set(MapDirection::EAST, TileEdge::ONE_WAY_HIDDEN_DOOR);
		adj_east.set(MapDirection::WEST, TileEdge::WALL);
		break;
	case 10:
		// One-way wall (exiting right or down)

		// Set east wall of current tile to One-way wall
		// Set west wall of adjacent tile to wall
		tile.set(MapDirection::EAST, TileEdge::ONE_WAY_WALL);
		adj_east.set(MapDirection::WEST, TileEdge::WALL);
		break;
	default:
		break;
	}
}

// Due to the way GC defines levels, we need to handle different edges differently so this is the inner function - this
// only works for simple non-directional wall-types - the complicated walls we handle later
auto Sorcery::Level::_convert_edge_simple(const unsigned int wall) const -> std::optional<TileEdge> {

	std::optional<TileEdge> edge{std::nullopt};
	switch (wall) { // NOLINT(clang-diagnostic-switch)
	case 0:
		edge = TileEdge::NO_EDGE;
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

auto Sorcery::Level::_convert_edge_se(const unsigned int wall) const -> std::optional<TileEdge> {

	std::optional<TileEdge> standard_edge{_convert_edge_simple(wall)};
	return standard_edge.value_or(TileEdge::NO_EDGE);
}

// Only populate walls that we need to populate at this point - those ones that have some meaning for N or W (i.e. types
// 5/6/7)
auto Sorcery::Level::_convert_edge_nw(const unsigned int wall) const -> std::optional<TileEdge> {

	std::optional<TileEdge> edge{std::nullopt};
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
	default:
		break;
	}

	return edge;
}
