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

#include "types/tile.hpp"
#include "common/enum.hpp"
#include "common/macro.hpp"
#include "types/error.hpp"

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

Sorcery::Tile::Tile(std::optional<Coordinate> location, std::optional<TLE> north, std::optional<TLE> south,
	std::optional<TLE> east, std::optional<TLE> west)
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
		Error error{SYE::OPTIONAL_RETURNED, e, "tile.location has no value!"};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Tile::has(const MAD direction) const -> bool {

	switch (direction) {
	case MAD::NORTH:
		return _north.has_value() ? (_north != TLE::NO_EDGE) : false;
		break;
	case MAD::SOUTH:
		return _south.has_value() ? (_south != TLE::NO_EDGE) : false;
		break;
	case MAD::EAST:
		return _east.has_value() ? (_east != TLE::NO_EDGE) : false;
		break;
	case MAD::WEST:
		return _west.has_value() ? (_west != TLE::NO_EDGE) : false;
		break;
	default:
		return false;
		break;
	}
}

auto Sorcery::Tile::has(const MAD direction, const TLE wall_type) const -> bool {

	switch (direction) {
	case MAD::NORTH:
		if (_north.has_value())
			return _north == wall_type;
		else
			return false;
		break;
	case MAD::SOUTH:
		if (_south.has_value())
			return _south == wall_type;
		else
			return false;
		break;
	case MAD::EAST:
		if (_east.has_value())
			return _east == wall_type;
		else
			return false;
		break;
	case MAD::WEST:
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

	_properties[unenum(TLP::EXPLORED)] = true;
}

auto Sorcery::Tile::has(const TLF feature) const -> bool {

	return _features[unenum(feature)];
}

auto Sorcery::Tile::is(const TLP property) const -> bool {

	return _properties[unenum(property)];
}

auto Sorcery::Tile::walkable(const MAD direction) const -> bool {

	auto edge{TLE::NO_EDGE};
	switch (direction) {
	case MAD::NORTH:
		edge = _north.value_or(TLE::NO_EDGE);
		break;
	case MAD::SOUTH:
		edge = _south.value_or(TLE::NO_EDGE);
		break;
	case MAD::EAST:
		edge = _east.value_or(TLE::NO_EDGE);
		break;
	case MAD::WEST:
		edge = _west.value_or(TLE::NO_EDGE);
		break;
	default:
		return false;
		break;
	}

	return (edge == TLE::SECRET_DOOR) || (edge == TLE::NO_EDGE) || (edge == TLE::UNLOCKED_DOOR) ||
		   (edge == TLE::ONE_WAY_DOOR) || (edge == TLE::ONE_WAY_HIDDEN_DOOR) || (edge == TLE::HIDDEN_DOOR) ||
		   (edge == TLE::ONE_WAY_WALL);
}

auto Sorcery::Tile::wall(const MAD direction) const -> TLE {

	switch (direction) {
	case MAD::NORTH:
		return _north.has_value() ? _north.value() : TLE::NO_EDGE;
		break;
	case MAD::SOUTH:
		return _south.has_value() ? _south.value() : TLE::NO_EDGE;
		break;
	case MAD::EAST:
		return _east.has_value() ? _east.value() : TLE::NO_EDGE;
		break;
	case MAD::WEST:
		return _west.has_value() ? _west.value() : TLE::NO_EDGE;
		break;
	default:
		return TLE::NO_EDGE;
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

auto Sorcery::Tile::reset(const TLF feature) -> void {

	_features[unenum(feature)] = false;
}

auto Sorcery::Tile::reset(const TLP property) -> void {

	_properties[unenum(property)] = false;
}

auto Sorcery::Tile::reset(const MAD direction) -> void {
	switch (direction) {

	case MAD::NORTH:
		_north = std::nullopt;
		break;
	case MAD::SOUTH:
		_south = std::nullopt;
		break;
	case MAD::EAST:
		_east = std::nullopt;
		break;
	case MAD::WEST:
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

auto Sorcery::Tile::set(const std::optional<MAV> event) -> void {

	_event = event;
}

auto Sorcery::Tile::set(const TLF feature) -> void {

	_features[unenum(feature)] = true;
}

auto Sorcery::Tile::set(const TLP property) -> void {

	_properties[unenum(property)] = true;
}

auto Sorcery::Tile::set(const MAD direction, const TLE new_wall) -> void {

	switch (direction) {
	case MAD::NORTH:
		_north = new_wall;
		break;
	case MAD::SOUTH:
		_south = new_wall;
		break;
	case MAD::EAST:
		_east = new_wall;
		break;
	case MAD::WEST:
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

auto Sorcery::Tile::has_event() const -> std::optional<MAV> {

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
		Error error{SYE::OPTIONAL_RETURNED, e, "tile.location.x has no value!"};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::Tile::y() const -> int {

	try {
		return _location.value().y;

	} catch (std::exception &e) {
		Error error{SYE::OPTIONAL_RETURNED, e, "tile.location.y has no value!"};
		std::cout << error;
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
