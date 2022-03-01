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

#include "tile.hpp"

// Default Constructor
Sorcery::Tile::Tile() {

	location = Point{0, 0};
	_reset_features();
	_reset_properties();
	_reset_metadata();
	_reset_walls();
}

// Standard Constructors
Sorcery::Tile::Tile(Point location_) : location{location_}, _id{s_id++} {

	_reset_features();
	_reset_properties();
	_reset_metadata();
	_reset_walls();
}

Sorcery::Tile::Tile(Point location_, std::map<TileWall, Wall> walls_)
	: location{location_}, walls{walls_}, _id{s_id++} {

	_reset_features();
	_reset_properties();
	_reset_metadata();
}

// Copy Constructors
Sorcery::Tile::Tile(const Tile &other)
	: location{other.location}, walls{other.walls}, floor{other.floor},
	  ceiling{other.ceiling}, features{other.features},
	  properties{other.properties}, description{other.description},
	  items{other.items}, events{other.events}, room_id{other.room_id},
	  treasure_id{other.treasure_id}, effect_id{other.effect_id},
	  characters{other.characters}, lighting{other.lighting}, _id{other._id} {}

auto Sorcery::Tile::operator=(const Tile &other) -> Tile & {

	location = other.location;
	walls = other.walls;
	floor = other.floor;
	ceiling = other.ceiling;
	features = std::move(other.features);
	properties = std::move(other.properties);
	description = other.description;
	items = std::move(other.items);
	events = std::move(other.events);
	room_id = other.room_id;
	treasure_id = other.treasure_id;
	effect_id = other.effect_id;
	characters = std::move(other.characters);
	lighting = other.lighting;

	_id = other._id;

	return *this;
}

// Move Constructors
Sorcery::Tile::Tile(Tile &&other) noexcept {

	if (this != &other) {
		location = other.location;
		walls = other.walls;
		floor = other.floor;
		ceiling = other.ceiling;
		features = other.features;
		properties = other.properties;
		description = other.description;
		items = other.items;
		events = other.events;
		room_id = other.room_id;
		treasure_id = other.treasure_id;
		effect_id = other.effect_id;
		characters = other.characters;
		lighting = other.lighting;

		_id = other._id;

		other.location = Point{0, 0};
		other.walls = {};
		other.floor = {};
		other.ceiling = {};
		other.features.clear();
		other.properties.clear();
		other.description = "";
		other.items.clear();
		other.events.clear();
		other.room_id = 0;
		other.treasure_id = 0;
		other.effect_id = 0;
		other.characters.clear();
		other.lighting = 0;

		other._id = 0;
	}
}

auto Sorcery::Tile::operator=(Tile &&other) noexcept -> Tile & {

	if (this != &other) {

		location = other.location;
		walls = other.walls;
		floor = other.floor;
		ceiling = other.ceiling;
		features = other.features;
		properties = other.properties;
		description = other.description;
		items = other.items;
		events = other.events;
		room_id = other.room_id;
		treasure_id = other.treasure_id;
		effect_id = other.effect_id;
		characters = other.characters;
		lighting = other.lighting;

		_id = other._id;

		other.location = Point{0, 0};
		other.walls = {};
		other.floor = {};
		other.ceiling = {};
		other.features.clear();
		other.properties.clear();
		other.description = "";
		other.items.clear();
		other.events.clear();
		other.room_id = 0;
		other.treasure_id = 0;
		other.effect_id = 0;
		other.characters.clear();
		other.lighting = 0;

		other._id = 0;
	}

	return *this;
}

auto Sorcery::Tile::set_explored() -> void {

	properties.at(TileProperty::EXPLORED) = true;
}

auto Sorcery::Tile::set_walkable() -> void {

	properties.at(TileProperty::WALKABLE) = true;
}

auto Sorcery::Tile::check_feature(const TileFeature value) const -> bool {

	return features.at(value);
}

auto Sorcery::Tile::check_property(const TileProperty value) const -> bool {

	return properties.at(value);
}

auto Sorcery::Tile::id() const -> long {

	return _id;
}

auto Sorcery::Tile::_reset_features() -> void {

	features.clear();
	features[TileFeature::FOUNTAIN] = false;
	features[TileFeature::MESSAGE] = false;
	features[TileFeature::MOVEMENT_NORTH] = false;
	features[TileFeature::MOVEMENT_SOUTH] = false;
	features[TileFeature::MOVEMENT_EAST] = false;
	features[TileFeature::MOVEMENT_WEST] = false;
	features[TileFeature::PIT] = false;
	features[TileFeature::POOL] = false;
	features[TileFeature::SPINNER] = false;
	features[TileFeature::TELEPORT] = false;
	features[TileFeature::STAIRS_DOWN] = false;
	features[TileFeature::STAIRS_UP] = false;
}

auto Sorcery::Tile::_reset_properties() -> void {

	properties.clear();
	properties[TileProperty::EXPLORED] = false;
	properties[TileProperty::DARKNESS] = false;
	properties[TileProperty::ROCK] = false;
	properties[TileProperty::WALKABLE] = false;
}

auto Sorcery::Tile::_reset_metadata() -> void {

	description = "";
	items.clear();
	events.clear();
	room_id = 0;
	treasure_id = 0;
	effect_id = 0;
	characters.clear();
	lighting = 255;
}

auto Sorcery::Tile::_reset_walls() -> void {

	walls.clear();
	walls[TileWall::NORTH] = Wall();
	walls[TileWall::SOUTH] = Wall();
	walls[TileWall::EAST] = Wall();
	walls[TileWall::WEST] = Wall();
	floor = FloorCeiling{};
	ceiling = FloorCeiling{};
}

auto Sorcery::Tile::check_wall(TileWall wall) -> bool {

	// TODO: Not entirely happy with this way of handling things
	return walls.at(wall).visible;
}

auto Sorcery::Tile::set_walls(bool north, bool south, bool east, bool west)
	-> void {

	walls.clear();
	if (north)
		walls[TileWall::NORTH] = Wall(MapDirection::NORTH);
	else
		walls[TileWall::NORTH] = Wall();
	if (south)
		walls[TileWall::SOUTH] = Wall(MapDirection::SOUTH);
	else
		walls[TileWall::SOUTH] = Wall();
	if (east)
		walls[TileWall::EAST] = Wall(MapDirection::EAST);
	else
		walls[TileWall::EAST] = Wall();
	if (west)
		walls[TileWall::WEST] = Wall(MapDirection::WEST);
	else
		walls[TileWall::WEST] = Wall();
}

auto Sorcery::Tile::set_wall_gfx(int value) -> void {

	if (walls.at(TileWall::NORTH).visible)
		walls.at(TileWall::NORTH).gfx = value;
	if (walls.at(TileWall::SOUTH).visible)
		walls.at(TileWall::SOUTH).gfx = value;
	if (walls.at(TileWall::EAST).visible)
		walls.at(TileWall::EAST).gfx = value;
	if (walls.at(TileWall::WEST).visible)
		walls.at(TileWall::WEST).gfx = value;
}

auto Sorcery::Tile::set_floor_gfx(int value) -> void {

	// For now, set them to the same
	floor.gfx = value;
	floor.visible = true;
	ceiling.gfx = value;
	ceiling.visible = true;
}

auto Sorcery::Tile::count_feature() const -> unsigned int {

	return std::count_if(
		std::begin(features), std::end(features), [=](auto value) {
			return value.second;
		});
}

auto Sorcery::Tile::count_property() const -> unsigned int {

	return std::count_if(
		std::begin(properties), std::end(properties), [=](auto &value) {
			return value.second;
		});
}
auto Sorcery::Tile::count_wall() const -> unsigned int {

	return std::count_if(std::begin(walls), std::end(walls), [=](auto &value) {
		return value.second.visible;
	});
}
