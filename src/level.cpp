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

#include "level.hpp"

// Default Constructor
Sorcery::Level::Level() {}

// Standard Constructor
Sorcery::Level::Level(const MapType type, const std::string dungeon,
	const int depth, const Coordinate bottom_left, const Size size)
	: _type{type}, _dungeon{dungeon}, _depth{depth},
	  _bottom_left{bottom_left}, _size{size} {}

// Copy Constructors
Sorcery::Level::Level(const Level &other)
	: _type{other._type}, _dungeon{other._dungeon}, _depth{_depth},
	  _bottom_left{_bottom_left}, _size{other._size} {}

auto Sorcery::Level::operator=(const Level &other) -> Level & {

	_type = other._type;
	_dungeon = other._dungeon;
	_depth = other._depth;
	_bottom_left = other._bottom_left;
	_size = other._size;

	return *this;
}

auto Sorcery::Level::load(const Json::Value row_data) -> bool {

	return true;
}

auto Sorcery::Level::name() const -> std::string {

	return _depth < 0 ? fmt::format("{} B{}F", _dungeon, std::abs(_depth))
					  : fmt::format("{} {}F", _dungeon, std::abs(_depth));
}
