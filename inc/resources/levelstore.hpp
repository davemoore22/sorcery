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

#include "core/system.hpp"
#include "resources/define.hpp"
#include "types/level.hpp"

namespace Sorcery {

class LevelStore {

	public:

		// Constructors
		LevelStore();
		LevelStore(System *system);
		LevelStore(System *system, const std::filesystem::path filename);

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(_loaded, _levels);
		}

		// Overload [] operator
		auto operator[](const int depth) const -> std::optional<Level>;

		// Public Methods
		auto set(System *system) -> void;

	private:

		// Private Members
		System *_system;
		bool _loaded;

		std::map<int, Level> _levels;

		// Private Methods
		auto _get(const int depth) const -> std::optional<Level>;
		auto _load(const std::filesystem::path filename) -> bool;
};

}