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

// Standard Constructor
Sorcery::Tile::Tile(sf::Vector2u location_, std::array<Wall, 4> walls_)
	: location{location_}, walls{walls_}, _id{s_id++} {

	features.clear();
	properties.clear();
	description = std::nullopt;
	items = std::nullopt;
	events = std::nullopt;
	room_id = std::nullopt;
	treasure_id = std::nullopt;
	effect_id = std::nullopt;
	characters = std::nullopt;
}

// Copy Constructors
Sorcery::Tile::Tile(const Tile &other)
	: location{other.location}, walls{other.walls}, features{other.features},
	  properties{other.properties}, description{other.description},
	  items{other.items}, events{other.events}, room_id{other.room_id},
	  treasure_id{other.treasure_id}, effect_id{other.effect_id},
	  characters{other.characters}, _id{other._id} {}

auto Sorcery::Tile::operator=(const Tile &other) -> Tile & {

	location = other.location;
	walls = other.walls;
	features = std::move(other.features);
	properties = std::move(other.properties);
	description = other.description;
	items = std::move(other.items);
	events = std::move(other.events);
	room_id = other.room_id;
	treasure_id = other.treasure_id;
	effect_id = other.effect_id;
	characters = std::move(other.characters);

	_id = other._id;

	return *this;
}

// Move Constructors
Sorcery::Tile::Tile(Tile &&other) noexcept {

	if (this != &other) {
		location = other.location;
		walls = other.walls;
		features = other.features;
		properties = other.properties;
		description = other.description;
		items = other.items;
		events = other.events;
		room_id = other.room_id;
		treasure_id = other.treasure_id;
		effect_id = other.effect_id;
		characters = other.characters;

		_id = other._id;

		other.location = sf::Vector2u(0, 0);
		other.walls = {};
		other.features.clear();
		other.properties.clear();
		other.description = std::nullopt;
		other.items = std::nullopt;
		other.events = std::nullopt;
		other.room_id = std::nullopt;
		other.treasure_id = std::nullopt;
		other.effect_id = std::nullopt;
		other.characters = std::nullopt;

		other._id = 0;
	}
}

auto Sorcery::Tile::operator=(Tile &&other) noexcept -> Tile & {

	if (this != &other) {

		location = other.location;
		walls = other.walls;
		features = other.features;
		properties = other.properties;
		description = other.description;
		items = other.items;
		events = other.events;
		room_id = other.room_id;
		treasure_id = other.treasure_id;
		effect_id = other.effect_id;
		characters = other.characters;

		_id = other._id;

		other.location = sf::Vector2u(0, 0);
		other.walls = {};
		other.features.clear();
		other.properties.clear();
		other.description = std::nullopt;
		other.items = std::nullopt;
		other.events = std::nullopt;
		other.room_id = std::nullopt;
		other.treasure_id = std::nullopt;
		other.effect_id = std::nullopt;
		other.characters = std::nullopt;

		other._id = 0;
	}

	return *this;
}

auto Sorcery::Tile::set_explored() -> void {

	properties.at(TileProperty::EXPLORED) = true;
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
