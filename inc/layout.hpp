// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.
#pragma once

#include "component.hpp"
#include "main.hpp"

// Class to handles managing screen layouts
namespace Sorcery {

	class Layout {

	  public:
		// Constructors
		Layout(const std::filesystem::path filename);
		Layout() = delete;

		// Overload [] operator
		auto operator[](const std::string &combined_key) -> Component &;
		auto operator()(const std::string &screen) -> std::optional<std::vector<Component>>;

		// Public Methods
		auto set_grid(unsigned int cell_width, unsigned int cell_height) -> void;

	  private:
		// Private Members
		std::map<std::string, Component> _components;
		bool _loaded;
		std::filesystem::file_time_type _last_modified;
		std::chrono::time_point<std::chrono::file_clock> _last_loaded;
		std::filesystem::path _filename;
		unsigned int _cell_width;
		unsigned int _cell_height;

		// Private Methods
		auto _load(const std::filesystem::path filename) -> bool;
		auto _refresh_needed() -> bool;
	};
} // namespace Sorcery
