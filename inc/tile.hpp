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
