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
#include "tile.hpp"

namespace Sorcery {

	class Map {

	  public:
		// Constructors
		Map();
		Map(MapType type);

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(_version, _tiles);
		}

		// Public Members

		// Public Methods
		auto at(Point loc) -> Tile &;
		auto at_relative(Point player_pos, int x, int y) -> Tile &;
		auto at(unsigned int x, unsigned int y) -> Tile &;
		auto tiles() const -> std::vector<Tile>;
		auto get_type() const -> MapType;
		auto recreate_level() -> void;

	  private:
		// Private Members
		int _version;
		std::vector<Tile> _tiles;
		MapType _type;

		// Private Methods
		auto _reset_level() -> void;
		auto _create_level(MapType type) -> void;
	};
} // namespace Sorcery