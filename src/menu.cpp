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

#include "menu.hpp"

// Standard Constructor
Sorcery::Menu::Menu(System& system, Display& display, Graphics& graphics, MenuType type, Component& component):
	_system {system}, _display {display}, _graphics {graphics}, _component {component} {

	// Get the Window and Graphics to Display
	_window = _display.window->get_window();
}

// Standard Destructor
Sorcery::Menu::~Menu() {
}

// Draw the Menu
auto Sorcery::Menu::draw() -> void {
}

// Add an item to the Menu
auto Sorcery::Menu::_add_item(int index, const MenuItemType itemtype, const MenuItem code, const std::string& key)
	-> void {
}

// Select the first enabled menu item
auto Sorcery::Menu::_select_first_enabled() -> void {
}

// Select the last enabled menu item
auto Sorcery::Menu::_select_last_enabled() -> void {
}




