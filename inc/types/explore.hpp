// Copyright (C) 2025 Dave Moore
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

#pragma once

#include "common/define.hpp"
#include "common/include.hpp"
#include "common/types.hpp"
#include "core/include.hpp"
#include "types/enum.hpp"

namespace Sorcery {

class Explore {

	public:
		// Constructors
		Explore();

		// Overload Operators
		auto operator[](Coordinate loc) const -> bool;

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(_tiles);
		}

		// Public Members

		// Public Methods
		auto at(const Coordinate loc) const -> bool;
		auto at(const int x, const int y) const -> bool;
		auto reset() -> void;
		auto set(const Coordinate loc) -> void;
		auto unset(const Coordinate loc) -> void;

	private:
		// Private Members
		std::map<Coordinate, bool> _tiles;
};
}