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

namespace Sorcery {

	class Menu {

		public:

			// Standard Constructor
			Menu(System& system, Display& display, Graphics& graphics, MenuType type);

			// Standard Destructor
			virtual ~Menu();

			// Overload [] operator
			auto operator[] (const unsigned int index) -> const MenuEntry&;

			// Public Members
			std::vector<MenuEntry> items;
			std::vector<sf::FloatRect> bounds;
			std::vector<MenuEntry>::const_iterator selected;
			unsigned int count;

			// Public Methods
			auto choose_next() -> std::optional<std::vector<MenuEntry>::const_iterator>;
			auto choose_previous() -> std::optional<std::vector<MenuEntry>::const_iterator>;
			auto choose(unsigned int index) -> std::optional<std::vector<MenuEntry>::const_iterator>;
			auto set_mouse_selected(sf::Vector2f mouse_position) ->
				std::optional<std::vector<MenuEntry>::const_iterator>;

		private:

			// Private Methods
			auto _add_item(int index, const MenuItemType itemtype, const MenuItem code, std::string& key,
				bool enabled = true) -> void;
			auto _select_first_enabled() -> void;
			auto _select_last_enabled() -> void;

			// Private Members
			System _system;
			Display _display;
			Graphics _graphics;
			sf::RenderWindow* _window;
			unsigned int _width;
			MenuType _type;

	};
}
