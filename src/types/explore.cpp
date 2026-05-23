// Copyright (C) 2026 Dave Moore
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

#include "types/explore.hpp"

// Default Constructor
Sorcery::Explore::Explore() {

	_tiles.clear();
}

auto Sorcery::Explore::at(Coordinate loc) const -> bool {
	return _tiles.contains(loc);
}

auto Sorcery::Explore::at(int x, int y) const -> bool {
	return at(Coordinate{x, y});
}

auto Sorcery::Explore::operator[](Coordinate loc) const -> bool {
	return at(loc);
}

auto Sorcery::Explore::set(Coordinate loc) -> void {
	_tiles.insert(loc);
}

auto Sorcery::Explore::reset() -> void {
	_tiles.clear();
}

auto Sorcery::Explore::unset(Coordinate loc) -> void {
	_tiles.erase(loc);
}