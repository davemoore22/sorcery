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

#include "attractmode.hpp"
#include "dialog.hpp"
#include "display.hpp"
#include "frame.hpp"
#include "game.hpp"
#include "graphics.hpp"
#include "layout.hpp"
#include "main.hpp"
#include "menu.hpp"
#include "system.hpp"

namespace Sorcery {

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
		auto start(MainMenuType menu_stage) -> std::optional<MenuItem>;
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
		MainMenuType _menu_stage;
		std::shared_ptr<Menu> _main_menu;
		std::shared_ptr<AttractMode> _attract_mode;
		WindowConfirm _yes_or_no;
		Component _attract_creatures_c;
		sf::Text _press_any_key;
		std::shared_ptr<Dialog> _dialog_new_game;
		std::shared_ptr<Dialog> _dialog_exit;
		std::optional<std::unique_ptr<Error>> _error;
	};
} // namespace Sorcery
