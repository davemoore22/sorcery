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
Sorcery::Menu::Menu(System& system, Display& display, Graphics& graphics, MenuType type): _system {system},
	_display {display}, _graphics {graphics}, _type {type} {

	// Get the Window and Graphics to Display#
	_window = _display.window->get_window();

	// Clear the Items
	items.clear();
	bounds.clear();
	count = 0;

	// Now depending on the menu type, add the relevant items
	switch (_type) {
	case MenuType::MAIN:
		_add_item(0, MenuItemType::ENTRY, MenuItem::MM_NEW_GAME, (*_display.string)["MAIN_MENU_OPTION_START"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::MM_CONTINUE_GAME, (*_display.string)["MAIN_MENU_OPTION_CONTINUE"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::MM_OPTIONS, (*_display.string)["MAIN_MENU_OPTION_OPTIONS"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::MM_COMPENDIUM, (*_display.string)["MAIN_MENU_OPTION_COMPENDIUM"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::MM_LICENSE, (*_display.string)["MAIN_MENU_OPTION_LICENSE"]);
		_add_item(5, MenuItemType::ENTRY, MenuItem::QUIT, (*_display.string)["MAIN_MENU_OPTION_QUIT"]);
		selected = items.begin();
		break;
	}
}

// Standard Destructor
Sorcery::Menu::~Menu() {
}

// Overload [] Operator
auto Sorcery::Menu::operator [] (const unsigned int index) -> const MenuEntry& {
	return items.at(index);
}

// Add an item to the Menu
auto Sorcery::Menu::_add_item(int index, const MenuItemType itemtype, const MenuItem code, std::string& key,
	bool enabled) -> void {
	if (key.length() % 2 == 0)
		key.resize(key.length() + 1, 32);
	items.push_back(std::tuple<unsigned int, Enums::Menu::ItemType, Enums::Menu::Item, std::string, bool>(index,
		itemtype, code, key, enabled));
	++count;
}

// Select the first enabled menu item
auto Sorcery::Menu::_select_first_enabled() -> void {
	std::vector<MenuEntry>::const_iterator it = {};
	for (it = items.begin(); it != items.end(); ++it)
		if (std::get<static_cast<int>(MenuField::TYPE)>(*it) == MenuItemType::ENTRY)
			if (std::get<static_cast<int>(MenuField::ENABLED)>(*it)) {
				selected = it;
				return;
			}
}

// Select the last enabled menu item
auto Sorcery::Menu::_select_last_enabled() -> void {
	std::vector<MenuEntry>::const_iterator it = {};
	for (it = items.end(); it != items.begin(); --it)
		if (std::get<static_cast<int>(MenuField::TYPE)>(*it) == MenuItemType::ENTRY)
			if (std::get<static_cast<int>(MenuField::ENABLED)>(*it)) {
				selected = it;
				return;
			}
}

// Check if the mouse cursor is on a menu item, and if so set it
auto Sorcery::Menu::set_mouse_selected(sf::Vector2f mouse_position) -> std::vector<MenuEntry>::const_iterator {
	bool found {false};
	std::vector<sf::FloatRect>::const_iterator working_bounds = {bounds.begin()};
	std::vector<MenuEntry>::const_iterator working_items = {items.begin()};
	do {
		if (working_bounds->contains(mouse_position)) {
			found = true;
			selected = working_items;
			return working_items;
		}

		++working_bounds;
		++working_items;
		} while ((working_bounds < bounds.end()) && (!found));

	// If we reach here the mouse cursor is outside the items so we don't do anything
}

// Set selected based upon the item index
auto Sorcery::Menu::choose(unsigned int index) -> std::vector<MenuEntry>::const_iterator {

	// Iterate through til we have found it
	if (index < items.size()) {
		bool found {false};
		std::vector<MenuEntry>::const_iterator working {items.begin()};
		do {
			found = std::get<static_cast<int>(MenuField::INDEX)>(*working) == index;
			++working;
		} while ((working > items.begin()) && (!found));
		if (found) {
			selected = working;
			return selected;
		}
	}
}

// Choose the previous selected item
auto Sorcery::Menu::choose_previous() -> void {
	if (selected > items.begin()) {

		// Iterate backwards until we find the first previous enabled menu if we can
		bool found_enabled_option {false};
		std::vector<MenuEntry>::const_iterator working {selected};
		do {
			--working;
			found_enabled_option = std::get<static_cast<int>(MenuField::ENABLED)>(*working) &&
				std::get<static_cast<int>(MenuField::TYPE)>(*working) == MenuItemType::ENTRY;
		} while ((working > items.begin()) && (!found_enabled_option));
		if (found_enabled_option)
			selected = working;
	}
}

// Choose the next selected item
auto Sorcery::Menu::choose_next() -> void {
	if (selected < items.end()) {

		// Iterate forwards until we find the first next enabled menu if we can
		bool found_enabled_option {false};
		std::vector<MenuEntry>::const_iterator working {selected};
		do {
			++working;
			found_enabled_option = std::get<static_cast<int>(MenuField::ENABLED)>(*working) &&
				std::get<static_cast<int>(MenuField::TYPE)>(*working) == MenuItemType::ENTRY;
		} while ((working < items.end() - 1) && (!found_enabled_option));
		if (found_enabled_option)
			selected = working;
	}
}