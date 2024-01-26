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

#pragma once

#include "common/include.hpp"
#include "common/type.hpp"
#include "types/item.hpp"
#include "types/type.hpp"

namespace Sorcery {

class Tile {

	public:

		// Constructors
		Tile();
		Tile(std::optional<Coordinate> location);
		Tile(std::optional<Coordinate> location, std::optional<TileEdge> north, std::optional<TileEdge> south,
			std::optional<TileEdge> east, std::optional<TileEdge> west);

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(_location, _north, _south, _east, _west, _texture_id, _properties, _features, _items, _event,
				_room_id, _treasure_id, _effect_id, _description_id, _characters, _lighting, _teleport, _stairs,
				_elevator, _id, s_id);
		}

		// No resources so no move/copy constructors needed
		// https://stackoverflow.com/questions/3510662/vectors-within-classes-handling-copy-constructor-and-destructor-c

		// Public Members

		// Public Methods
		auto clear_teleport() -> void;
		auto clear_stairs() -> void;
		auto gfx(const unsigned int texture) -> void;
		auto gfx() -> std::optional<unsigned int>;
		auto has(const MapDirection direction) const -> bool;
		auto has(const MapDirection direction, const TileEdge wall_type) const -> bool;
		auto has(const TileFeature feature) const -> bool;
		auto has_event() const -> std::optional<MapEvent>;
		auto has_stairs() const -> std::optional<Teleport>;
		auto has_teleport() const -> std::optional<Teleport>;
		auto has_elevator() const -> std::optional<Elevator>;
		auto id() const -> long;
		auto is(const TileProperty property) const -> bool;
		auto loc() const -> Coordinate;
		auto reset() -> void;
		auto reset(const TileFeature feature) -> void;
		auto reset(const TileProperty property) -> void;
		auto reset(const MapDirection direction) -> void;
		auto set(const std::optional<MapEvent> event) -> void;
		auto set(const TileFeature feature) -> void;
		auto set(const TileProperty property) -> void;
		auto set(const MapDirection direction, TileEdge new_wall) -> void;
		auto set(const std::optional<Coordinate> location);
		auto set_explored() -> void;
		auto set_teleport(Teleport teleport) -> void;
		auto set_stairs(Teleport stairs) -> void;
		auto set_elevator(Elevator elevator) -> void;
		auto wall(const MapDirection direction) const -> TileEdge;
		auto walkable(const MapDirection direction) const -> bool;
		auto x() const -> int;
		auto y() const -> int;

		// need methods for items, events etc
		std::vector<Item> items;

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
		std::bitset<10> _properties;

		// Features
		std::bitset<32> _features;

		// Items
		std::vector<unsigned int> _items;

		// Event
		std::optional<MapEvent> _event;

		// Various IDs
		std::optional<unsigned int> _room_id;
		std::optional<unsigned int> _treasure_id;
		std::optional<unsigned int> _effect_id;
		std::optional<unsigned int> _description_id;

		// Teleport
		std::optional<Teleport> _teleport;

		// Stairs
		std::optional<Teleport> _stairs;

		// Elevator
		std::optional<Elevator> _elevator;

		// Characters here (not the current party)
		std::vector<unsigned int> _characters;

		// Graphics Effects
		std::optional<unsigned int> _lighting;

		long _id;
		static inline long s_id{0};
};

}
