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
class AttractMode;
class Dialog;
class Display;
class Factory;
class Game;
class Graphics;
class Menu;
class System;
class Text;

class MainMenu {

	public:

		// Constructors
		MainMenu(
			System *system, Display *display, Graphics *graphics, Game *game);
		MainMenu() = delete;

		// Standard Destructor
		virtual ~MainMenu();

		// Public Members

		// Public Methods
		auto start(MMT menu_stage) -> std::optional<MIM>;
		auto stop() -> void;

	private:

		// Private Methods
		auto _draw() -> void;
		auto _set() -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		sf::RenderWindow *_window;
		MMT _menu_stage;
		std::unique_ptr<Menu> _main_menu;
		std::unique_ptr<AttractMode> _attract_mode;
		Component _attract_creatures_c;
		std::unique_ptr<Text> _press_any_key;
		std::unique_ptr<Dialog> _dialog_new_game;
		std::unique_ptr<Dialog> _dialog_exit;
		std::optional<std::unique_ptr<Error>> _error;
		std::unique_ptr<Factory> _factory;
};

}
