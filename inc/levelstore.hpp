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

#include "level.hpp"
#include "main.hpp"
#include "system.hpp"

namespace Sorcery {

	class LevelStore {

	  public:
		// Constructors
		LevelStore(System *system, const std::filesystem::path filename);
		LevelStore() = delete;

		// Overload [] operator
		auto operator[](const std::string name) const -> std::optional<Level>;

		// Public Methods

	  private:
		// Private Members
		System *_system;
		bool _loaded;

		std::map<std::string, Level> _levels;

		// Private Methods
		auto _get(const std::string name) const -> std::optional<Level>;
		auto _load(const std::filesystem::path filename) -> bool;
	};
} // namespace Sorcery