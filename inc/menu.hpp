// Copyright (C) 2020 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#pragma once

#include "main.hpp"
#include "system.hpp"
#include "display.hpp"
#include "graphics.hpp"
#include "component.hpp"


namespace Sorcery {

	class Menu {

		public:

			// Standard Constructor
			Menu(System& system, Display& display, Graphics& graphics, MenuType type, Component& component);

			// Standard Destructor
			virtual ~Menu();

			// Public Members
			MenuItem selected_item; // Currently selected menu item

			// Public Methods
			auto draw() -> void;

		private:

			// Private Methods
			auto _add_item(int index, const MenuItemType itemtype, const MenuItem code, const std::string& key) -> void;
			auto _select_first_enabled() -> void;
			auto _select_last_enabled() -> void;

			// Private Members
			MenuType _type;
			System _system;
			Display _display;
			Graphics _graphics;
			Component _component;
			sf::RenderWindow* _window;
			std::vector<MenuEntry> _items;
	};
}
