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

namespace Sorcery {

// Forward Declarations
class Dialog;
class Display;
class Frame;
class Game;
class Graphics;
class Menu;
class System;

class Factory {

	public:

		// Constructor
		Factory(System *system, Display *display, Graphics *graphics, Game *game);
		Factory() = delete;

		// Public Members

		// Public Methods
		auto make_dialog(const std::string &component, const WDT type = WDT::OK, const unsigned int duration = 0)
			-> std::unique_ptr<Dialog>;
		auto make_menu(const std::string &component, const MenuType type, std::optional<MenuMode> mode = std::nullopt,
			std::optional<unsigned int> data = 0, const bool reload = false) -> std::unique_ptr<Menu>;
		auto make_frame(const std::string &component) -> std::unique_ptr<Frame>;
		auto make_menu_frame(const std::string &component) -> std::unique_ptr<Frame>;

	private:

		// Private Methods

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
};

}