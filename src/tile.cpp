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

#include "tile.hpp"

// Default Constructor
Sorcery::Tile::Tile() {

	_location = std::nullopt;

	_north = std::nullopt;
	_south = std::nullopt;
	_east = std::nullopt;
	_west = std::nullopt;

	_reset();

	// TODO
	_texture_id = 5;

	s_id++;
}

// Other Constructors
Sorcery::Tile::Tile(const std::optional<Coordinate> location) : _location{location} {

	_north = std::nullopt;
	_south = std::nullopt;
	_east = std::nullopt;
	_west = std::nullopt;

	_reset();

	// TODO
	_texture_id = 5;

	s_id++;
}

Sorcery::Tile::Tile(std::optional<Coordinate> location, std::optional<TileEdge> north, std::optional<TileEdge> south,
	std::optional<TileEdge> east, std::optional<TileEdge> west)
	: _location{location}, _north{north}, _south{south}, _east{east}, _west{west} {

	_reset();

	// TODO
	_texture_id = 5;

	s_id++;
}

auto Sorcery::Tile::loc() const -> Coordinate {

	try {
		return _location.value();

	} catch (std::exception &e) {
		Error error{SystemError::OPTIONAL_RETURNED, e, "tile.location has no value!"};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Tile::has(const MapDirection direction) const -> bool {

	switch (direction) {
	case MapDirection::NORTH:
		return _north.has_value() ? (_north != TileEdge::NO_EDGE) : false;
		break;
	case MapDirection::SOUTH:
		return _south.has_value() ? (_south != TileEdge::NO_EDGE) : false;
		break;
	case MapDirection::EAST:
		return _east.has_value() ? (_east != TileEdge::NO_EDGE) : false;
		break;
	case MapDirection::WEST:
		return _west.has_value() ? (_west != TileEdge::NO_EDGE) : false;
		break;
	default:
		return false;
		break;
	}
}

auto Sorcery::Tile::has(const MapDirection direction, const TileEdge wall_type) const -> bool {

	switch (direction) {
	case MapDirection::NORTH:
		if (_north.has_value())
			return _north == wall_type;
		else
			return false;
		break;
	case MapDirection::SOUTH:
		if (_south.has_value())
			return _south == wall_type;
		else
			return false;
		break;
	case MapDirection::EAST:
		if (_east.has_value())
			return _east == wall_type;
		else
			return false;
		break;
	case MapDirection::WEST:
		if (_west.has_value())
			return _west == wall_type;
		else
			return false;
		break;
	default:
		return false;
		break;
	}
}

auto Sorcery::Tile::set_explored() -> void {

	_properties[magic_enum::enum_integer<TileProperty>(TileProperty::EXPLORED)] = true;
}

auto Sorcery::Tile::has(const TileFeature feature) const -> bool {

	return _features[magic_enum::enum_integer<TileFeature>(feature)];
}

auto Sorcery::Tile::is(const TileProperty property) const -> bool {

	return _properties[magic_enum::enum_integer<TileProperty>(property)];
}

auto Sorcery::Tile::walkable(const MapDirection direction) const -> bool {

	auto edge{TileEdge::NO_EDGE};
	switch (direction) {
	case MapDirection::NORTH:
		edge = _north.value_or(TileEdge::NO_EDGE);
		break;
	case MapDirection::SOUTH:
		edge = _south.value_or(TileEdge::NO_EDGE);
		break;
	case MapDirection::EAST:
		edge = _east.value_or(TileEdge::NO_EDGE);
		break;
	case MapDirection::WEST:
		edge = _west.value_or(TileEdge::NO_EDGE);
		break;
	default:
		return false;
		break;
	}

	return (edge == TileEdge::SECRET_DOOR) || (edge == TileEdge::NO_EDGE) || (edge == TileEdge::UNLOCKED_DOOR) ||
		   (edge == TileEdge::ONE_WAY_DOOR) || (edge == TileEdge::ONE_WAY_HIDDEN_DOOR) ||
		   (edge == TileEdge::HIDDEN_DOOR) || (edge == TileEdge::ONE_WAY_WALL);
}

auto Sorcery::Tile::wall(const MapDirection direction) const -> TileEdge {

	switch (direction) {
	case MapDirection::NORTH:
		return _north.has_value() ? _north.value() : TileEdge::NO_EDGE;
		break;
	case MapDirection::SOUTH:
		return _south.has_value() ? _south.value() : TileEdge::NO_EDGE;
		break;
	case MapDirection::EAST:
		return _east.has_value() ? _east.value() : TileEdge::NO_EDGE;
		break;
	case MapDirection::WEST:
		return _west.has_value() ? _west.value() : TileEdge::NO_EDGE;
		break;
	default:
		return TileEdge::NO_EDGE;
		break;
	}
}

auto Sorcery::Tile::id() const -> long {

	return _id;
}

auto Sorcery::Tile::reset() -> void {

	_location = std::nullopt;

	_north = std::nullopt;
	_south = std::nullopt;
	_east = std::nullopt;
	_west = std::nullopt;

	_reset();
}

auto Sorcery::Tile::reset(const TileFeature feature) -> void {

	_features[magic_enum::enum_integer<TileFeature>(feature)] = false;
}

auto Sorcery::Tile::reset(const TileProperty property) -> void {

	_properties[magic_enum::enum_integer<TileProperty>(property)] = false;
}

auto Sorcery::Tile::reset(const MapDirection direction) -> void {
	switch (direction) {

	case MapDirection::NORTH:
		_north = std::nullopt;
		break;
	case MapDirection::SOUTH:
		_south = std::nullopt;
		break;
	case MapDirection::EAST:
		_east = std::nullopt;
		break;
	case MapDirection::WEST:
		_west = std::nullopt;
		break;
	default:
		break;
	}
}

auto Sorcery::Tile::gfx(const unsigned int texture) -> void {

	_texture_id = texture;
}

auto Sorcery::Tile::gfx() -> std::optional<unsigned int> {

	return _texture_id;
}

auto Sorcery::Tile::set(const TileFeature feature) -> void {

	_features[magic_enum::enum_integer<TileFeature>(feature)] = true;
}

auto Sorcery::Tile::set(const TileProperty property) -> void {

	_properties[magic_enum::enum_integer<TileProperty>(property)] = true;
}

auto Sorcery::Tile::set(const MapDirection direction, const TileEdge new_wall) -> void {

	switch (direction) {
	case MapDirection::NORTH:
		_north = new_wall;
		break;
	case MapDirection::SOUTH:
		_south = new_wall;
		break;
	case MapDirection::EAST:
		_east = new_wall;
		break;
	case MapDirection::WEST:
		_west = new_wall;
		break;
	default:
		break;
	}
}

auto Sorcery::Tile::clear_teleport() -> void {

	_teleport = std::nullopt;
}

auto Sorcery::Tile::clear_stairs() -> void {

	_stairs = std::nullopt;
}

auto Sorcery::Tile::has_teleport() const -> std::optional<Teleport> {

	if (_teleport)
		return _teleport.value();
	else
		return std::nullopt;
}

auto Sorcery::Tile::has_stairs() const -> std::optional<Teleport> {

	if (_stairs)
		return _stairs.value();
	else
		return std::nullopt;
}

auto Sorcery::Tile::set_teleport(Teleport teleport) -> void {

	_teleport = teleport;
}

auto Sorcery::Tile::set_stairs(Teleport stairs) -> void {

	_stairs = stairs;
}

auto Sorcery::Tile::has_elevator() const -> std::optional<Elevator> {

	if (_elevator)
		return _elevator.value();
	else
		return std::nullopt;
}

auto Sorcery::Tile::set_elevator(Elevator elevator) -> void {

	_elevator = elevator;
}

auto Sorcery::Tile::set(const std::optional<Coordinate> location) {

	_location = location;
}

auto Sorcery::Tile::x() const -> int {

	try {
		return _location.value().x;

	} catch (std::exception &e) {
		Error error{SystemError::OPTIONAL_RETURNED, e, "tile.location.x has no value!"};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Tile::y() const -> int {

	try {
		return _location.value().y;

	} catch (std::exception &e) {
		Error error{SystemError::OPTIONAL_RETURNED, e, "tile.location.y has no value!"};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Tile::_reset() -> void {

	_texture_id = std::nullopt;

	_properties.reset();
	_features.reset();

	_items.clear();
	_events.clear();

	_room_id = std::nullopt;
	_treasure_id = std::nullopt;
	_effect_id = std::nullopt;
	_description_id = std::nullopt;

	_teleport = std::nullopt;
	_stairs = std::nullopt;
	_elevator = std::nullopt;

	_characters.clear();

	_lighting = std::nullopt;
}
