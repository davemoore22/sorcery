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

#pragma once

#include "main.hpp"

namespace Sorcery {

	class Tile_ {

	  public:
		// Constructors
		Tile_();
		Tile_(std::optional<Coordinate> location);
		Tile_(std::optional<Coordinate> location, std::optional<TileEdge> north,
			std::optional<TileEdge> south, std::optional<TileEdge> east,
			std::optional<TileEdge> west);

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(_location, _north, _south, _east, _west, _texture_id,
				_properties, _features, _items, _events, _room_id, _treasure_id,
				_effect_id, _description_id, _characters, _lighting, _id, s_id);
		}

		// No resources so no move/copy constructors needed
		// https://stackoverflow.com/questions/3510662/vectors-within-classes-handling-copy-constructor-and-destructor-c

		// Public Members

		// Public Method
		auto loc() const -> Coordinate;
		auto has(const MapDirection direction) const -> bool;
		auto has(const TileFeature feature) const -> bool;
		auto is(const TileProperty property) const -> bool;
		auto id() const -> long;
		auto reset() -> void;
		auto reset(const TileFeature feature) -> void;
		auto reset(const TileProperty property) -> void;
		auto reset(const MapDirection direction) -> void;
		auto set(const TileFeature feature) -> void;
		auto set(const TileProperty property) -> void;
		auto set(const MapDirection direction, TileEdge new_wall) -> void;
		auto set(const std::optional<TileEdge> north,
			const std::optional<TileEdge> south,
			const std::optional<TileEdge> east,
			const std::optional<TileEdge> west) -> void;
		auto set(const std::optional<Coordinate> location);
		auto x() const -> int;
		auto y() const -> int;

		// need methods for items, events etc

	  private:
		// Private Methods
		auto _reset() -> void;

		// Private Members
		std::optional<Coordinate> _location;

		// Walls (based upon https://docs.gridcartographer.com/ref/table/edge)
		std::optional<TileEdge> _north;
		std::optional<TileEdge> _south;
		std::optional<TileEdge> _east;
		std::optional<TileEdge> _west;

		// Texture
		std::optional<unsigned int> _texture_id;

		// Properties
		std::bitset<4> _properties;

		// Features
		std::bitset<13> _features;

		// Items and Events
		std::vector<unsigned int> _items;
		std::vector<unsigned int> _events;

		// Various IDs
		std::optional<unsigned int> _room_id;
		std::optional<unsigned int> _treasure_id;
		std::optional<unsigned int> _effect_id;
		std::optional<unsigned int> _description_id;

		// Characters here (not the current party)
		std::vector<unsigned int> _characters;

		// Graphics Effects
		std::optional<unsigned int> _lighting;

		long _id;
		static inline long s_id{0};
	};

	class Tile {

	  public:
		// Default Constructor
		Tile();

		// Standard Constructors
		Tile(Point location_);
		Tile(Point location_, std::map<TileWall, Wall> walls_);

		// Copy Constructors
		Tile(const Tile &other);
		auto operator=(const Tile &other) -> Tile &;

		// Move Constructors
		Tile(Tile &&other) noexcept;
		auto operator=(Tile &&other) noexcept -> Tile &;

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(location, walls, floor, ceiling, features, properties,
				description, items, events, room_id, treasure_id, effect_id,
				characters, lighting, _id, s_id);
		}

		// Public Members
		Point location;
		std::map<TileWall, Wall> walls;
		FloorCeiling floor;
		FloorCeiling ceiling;
		std::map<TileFeature, bool> features;
		std::map<TileProperty, bool> properties;
		std::string description;
		std::vector<unsigned int> items;
		std::vector<unsigned int> events;
		unsigned int room_id;
		unsigned int treasure_id;
		unsigned int effect_id;
		std::vector<unsigned int> characters;
		unsigned int lighting;

		// Public Methods
		auto set_explored() -> void;
		auto set_walkable() -> void;
		auto check_feature(const TileFeature value) const -> bool;
		auto check_property(const TileProperty value) const -> bool;
		auto check_wall(TileWall wall) -> bool;
		auto count_feature() const -> unsigned int;
		auto count_property() const -> unsigned int;
		auto count_wall() const -> unsigned int;
		auto id() const -> long;
		auto set_walls(bool north, bool south, bool east, bool west) -> void;
		auto set_walls_mask(bool north, bool south, bool east, bool west)
			-> void;
		auto set_wall_gfx(int value) -> void;
		auto set_floor_gfx(int value) -> void;

	  private:
		// Private Methods
		auto _reset_features() -> void;
		auto _reset_metadata() -> void;
		auto _reset_properties() -> void;
		auto _reset_walls() -> void;

		// Private Members
		long _id;
		static inline long s_id{0};
	};
} // namespace Sorcery
