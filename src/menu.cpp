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
	_items.clear();
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
		_selected = _items.begin();
		break;
	}
}

// Standard Destructor
Sorcery::Menu::~Menu() {
}

// Overload [] Operator
auto Sorcery::Menu::operator [] (const unsigned int index) -> const MenuEntry& {
	return _items.at(index);
}

// Draw the Menu
auto Sorcery::Menu::draw(Component& component, double lerp) -> void {

	_width = component.width;

	int x {0};
	int y {0};
	int count {0};
	sf::Text text;

	text.setFont(_system.resources->fonts[component.font]);
	text.setCharacterSize(component.size);

	std::vector<MenuEntry>::const_iterator it = {};
	for (it = _items.begin(); it != _items.end(); ++it) {
		std::string text_string {std::get<static_cast<int>(MenuField::TEXT)>(*it)};
		text.setFillColor(sf::Color(component.colour));
		text.setString(text_string);
		x = component.x == -1 ? _display.window->centre.x :  component.x;
		y = component.y == -1 ? _display.window->centre.y :  component.y;
		y = y + (count * _display.window->get_cell_height());
		text.setPosition(x, y);
		if (_selected == it) {
			sf::FloatRect background_rect {text.getLocalBounds()};
			sf::RectangleShape background(sf::Vector2f(_width * _display.window->get_cell_width(),
				background_rect.height + 2));
			background.setOrigin(background.getGlobalBounds().width / 2.0f, -4);
			if ((component.animated) && (lerp >= 0.0l))
				background.setFillColor(_display.window->change_colour(sf::Color(32, 158, 32), lerp));
			else
				background.setFillColor(sf::Color(32, 158, 32));
			text.setFillColor(sf::Color(0, 0, 0));


			_display.window->get_window()->draw(background, text.getTransform());
		}
		text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
		_display.window->draw_centered_text(text);
		count++;
	}
}

// Add an item to the Menu
auto Sorcery::Menu::_add_item(int index, const MenuItemType itemtype, const MenuItem code, std::string& key,
	bool enabled) -> void {
	if (key.length() % 2 == 0)
		key.resize(key.length() + 1, 32);
	_items.push_back(std::tuple<unsigned int, Enums::Menu::ItemType, Enums::Menu::Item, std::string, bool>(index,
		itemtype, code, key, enabled));
	++count;
}

// Select the first enabled menu item
auto Sorcery::Menu::_select_first_enabled() -> void {
	std::vector<MenuEntry>::const_iterator it = {};
	for (it = _items.begin(); it != _items.end(); ++it)
		if (std::get<static_cast<int>(MenuField::TYPE)>(*it) == MenuItemType::ENTRY)
			if (std::get<static_cast<int>(MenuField::ENABLED)>(*it)) {
				_selected = it;
				return;
			}
}

// Select the last enabled menu item
auto Sorcery::Menu::_select_last_enabled() -> void {
	std::vector<MenuEntry>::const_iterator it = {};
	for (it = _items.end(); it != _items.begin(); --it)
		if (std::get<static_cast<int>(MenuField::TYPE)>(*it) == MenuItemType::ENTRY)
			if (std::get<static_cast<int>(MenuField::ENABLED)>(*it)) {
				_selected = it;
				return;
			}
}




