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

#include "types/tile.hpp"
#include "common/enum.hpp"
#include "common/macro.hpp"

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
Sorcery::Tile::Tile(const std::optional<Coordinate> location)
	: _location{location} {

	_north = std::nullopt;
	_south = std::nullopt;
	_east = std::nullopt;
	_west = std::nullopt;

	_reset();

	// TODO
	_texture_id = 5;

	s_id++;
}

Sorcery::Tile::Tile(std::optional<Coordinate> location,
					std::optional<Enums::Tile::Edge> north,
					std::optional<Enums::Tile::Edge> south,
					std::optional<Enums::Tile::Edge> east,
					std::optional<Enums::Tile::Edge> west)
	: _location{location},
	  _north{north},
	  _south{south},
	  _east{east},
	  _west{west} {

	_reset();

	// TODO
	_texture_id = 5;

	s_id++;
}

auto Sorcery::Tile::loc() const -> Coordinate {

	try {
		return _location.value();

	} catch (std::exception &e) {
		Error error{Enums::System::Error::OPTIONAL_RETURNED, e,
					"tile.location has no value !"};
		std::cerr << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Tile::has(const Enums::Map::Direction direction) const -> bool {

	switch (direction) {
	case Enums::Map::Direction::NORTH:
		return _north.has_value() ? (_north != Enums::Tile::Edge::NO_EDGE)
								  : false;
		break;
	case Enums::Map::Direction::SOUTH:
		return _south.has_value() ? (_south != Enums::Tile::Edge::NO_EDGE)
								  : false;
		break;
	case Enums::Map::Direction::EAST:
		return _east.has_value() ? (_east != Enums::Tile::Edge::NO_EDGE)
								 : false;
		break;
	case Enums::Map::Direction::WEST:
		return _west.has_value() ? (_west != Enums::Tile::Edge::NO_EDGE)
								 : false;
		break;
	default:
		return false;
		break;
	}
}

auto Sorcery::Tile::has(const Enums::Map::Direction direction,
						const Enums::Tile::Edge wall_type) const -> bool {

	switch (direction) {
	case Enums::Map::Direction::NORTH:
		if (_north.has_value())
			return _north == wall_type;
		else
			return false;
		break;
	case Enums::Map::Direction::SOUTH:
		if (_south.has_value())
			return _south == wall_type;
		else
			return false;
		break;
	case Enums::Map::Direction::EAST:
		if (_east.has_value())
			return _east == wall_type;
		else
			return false;
		break;
	case Enums::Map::Direction::WEST:
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

	_properties[unenum(Enums::Tile::Properties::EXPLORED)] = true;
}

auto Sorcery::Tile::has(const Enums::Tile::Features feature) const -> bool {

	return _features[unenum(feature)];
}

auto Sorcery::Tile::is(const Enums::Tile::Properties property) const -> bool {

	return _properties[unenum(property)];
}

auto Sorcery::Tile::walkable(const Enums::Map::Direction direction) const
	-> bool {

	auto edge{Enums::Tile::Edge::NO_EDGE};
	switch (direction) {
	case Enums::Map::Direction::NORTH:
		edge = _north.value_or(Enums::Tile::Edge::NO_EDGE);
		break;
	case Enums::Map::Direction::SOUTH:
		edge = _south.value_or(Enums::Tile::Edge::NO_EDGE);
		break;
	case Enums::Map::Direction::EAST:
		edge = _east.value_or(Enums::Tile::Edge::NO_EDGE);
		break;
	case Enums::Map::Direction::WEST:
		edge = _west.value_or(Enums::Tile::Edge::NO_EDGE);
		break;
	default:
		return false;
		break;
	}

	return (edge == Enums::Tile::Edge::SECRET_DOOR) ||
		   (edge == Enums::Tile::Edge::NO_EDGE) ||
		   (edge == Enums::Tile::Edge::UNLOCKED_DOOR) ||
		   (edge == Enums::Tile::Edge::ONE_WAY_DOOR) ||
		   (edge == Enums::Tile::Edge::ONE_WAY_HIDDEN_DOOR) ||
		   (edge == Enums::Tile::Edge::HIDDEN_DOOR) ||
		   (edge == Enums::Tile::Edge::ONE_WAY_WALL);
}

auto Sorcery::Tile::wall(const Enums::Map::Direction direction) const
	-> Enums::Tile::Edge {

	switch (direction) {
	case Enums::Map::Direction::NORTH:
		return _north.has_value() ? _north.value() : Enums::Tile::Edge::NO_EDGE;
		break;
	case Enums::Map::Direction::SOUTH:
		return _south.has_value() ? _south.value() : Enums::Tile::Edge::NO_EDGE;
		break;
	case Enums::Map::Direction::EAST:
		return _east.has_value() ? _east.value() : Enums::Tile::Edge::NO_EDGE;
		break;
	case Enums::Map::Direction::WEST:
		return _west.has_value() ? _west.value() : Enums::Tile::Edge::NO_EDGE;
		break;
	default:
		return Enums::Tile::Edge::NO_EDGE;
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

auto Sorcery::Tile::reset(const Enums::Tile::Features feature) -> void {

	_features[unenum(feature)] = false;
}

auto Sorcery::Tile::reset(const Enums::Tile::Properties property) -> void {

	_properties[unenum(property)] = false;
}

auto Sorcery::Tile::reset(const Enums::Map::Direction direction) -> void {
	switch (direction) {

	case Enums::Map::Direction::NORTH:
		_north = std::nullopt;
		break;
	case Enums::Map::Direction::SOUTH:
		_south = std::nullopt;
		break;
	case Enums::Map::Direction::EAST:
		_east = std::nullopt;
		break;
	case Enums::Map::Direction::WEST:
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

auto Sorcery::Tile::set(const std::optional<Enums::Map::Event> event) -> void {

	_event = event;
}

auto Sorcery::Tile::set(const Enums::Tile::Features feature) -> void {

	_features[unenum(feature)] = true;
}

auto Sorcery::Tile::set(const Enums::Tile::Properties property) -> void {

	_properties[unenum(property)] = true;
}

auto Sorcery::Tile::set(const Enums::Map::Direction direction,
						const Enums::Tile::Edge new_wall) -> void {

	switch (direction) {
	case Enums::Map::Direction::NORTH:
		_north = new_wall;
		break;
	case Enums::Map::Direction::SOUTH:
		_south = new_wall;
		break;
	case Enums::Map::Direction::EAST:
		_east = new_wall;
		break;
	case Enums::Map::Direction::WEST:
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

auto Sorcery::Tile::has_event() const -> std::optional<Enums::Map::Event> {

	if (_event)
		return _event.value();
	else
		return std::nullopt;
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
		Error error{Enums::System::Error::OPTIONAL_RETURNED, e,
					"tile.location.x has no value!"};
		std::cerr << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Tile::y() const -> int {

	try {
		return _location.value().y;

	} catch (std::exception &e) {
		// error{SYE::OPTIONAL_RETURNED, e, "tile.location.y has no
		// value!"}; std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Tile::_reset() -> void {

	_texture_id = std::nullopt;

	_properties.reset();
	_features.reset();

	_items.clear();

	_event = std::nullopt;

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
