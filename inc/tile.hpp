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

	struct Door {

		DoorType type;
		bool secret;
		std::optional<unsigned int> gfx;
	};

	struct Wall {

		bool visible;
		bool walkable;
		MapDirection direction;
		std::optional<unsigned int> gfx;
		std::optional<Door> door;
	};

	class Tile {

		// Standard Constructor
		Tile(sf::Vector2u location, std::array<Wall, 4> walls);
		Tile() = delete;

		// Standard Destructor
		~Tile();

		// Copy Constructors
		Tile(const Tile &other);
		auto operator=(const Tile &other) -> Tile &;

		// Move Constructors
		Tile(Tile &&other) noexcept;
		auto operator=(Tile &&other) noexcept -> Tile &;

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(tile_id, location, walls, features, properties, description,
				items, events, room_id, treasure_id, effect_id, characters);
		}

	  public:
		// Public Members
		unsigned int tile_id;
		sf::Vector2u location;
		std::array<Wall, 4> walls;
		std::map<TileFeature, bool> features;
		std::map<TileProperty, bool> properties;
		std::optional<std::string> description;
		std::optional<std::vector<unsigned int>> items;
		std::optional<std::vector<unsigned int>> events;
		std::optional<unsigned int> room_id;
		std::optional<unsigned int> treasure_id;
		std::optional<unsigned int> effect_id;
		std::optional<std::vector<unsigned int>> characters;

		// Public Methods
		auto set_explored() -> void;
		auto check_feature(const TileFeature value) const -> bool;
		auto check_property(const TileProperty value) const -> bool;

	  private:
		// Private Methods
		// Private Members
	};
} // namespace Sorcery