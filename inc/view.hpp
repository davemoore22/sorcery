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

#include "display.hpp"
#include "graphics.hpp"
#include "main.hpp"
#include "system.hpp"

// Class to handles managing screen layouts
namespace Sorcery {

	class View {

	  public:
		// Constructors
		View(System *system, Display *display, Graphics *graphics);
		View() = delete;

		// Public Methods
		auto get(int x, int z) -> ViewNode &;

		// Operator Overloading
		auto operator[](Coordinate3 point) -> ViewNode &;
		auto operator[](int z) -> std::vector<ViewNode *>;

	  private:
		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		std::map<Coordinate3, ViewNode> _nodes;
		bool _loaded;

		// Private Methods
		auto _get(int x, int z) -> ViewNode &;
		auto _load(const std::filesystem::path filename) -> bool;
	};
} // namespace Sorcery
