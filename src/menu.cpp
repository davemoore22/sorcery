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
		_add_item(5, MenuItemType::ENTRY, MenuItem::QUIT, (*_display.string)["MAIN_MENU_OPTION_EXIT"]);
		selected = items.begin();
		break;
	case MenuType::ALLOCATE_CHARACTER_ATTRIBUTES:
		break;
	case MenuType::CHOOSE_CHARACTER_ALIGNMENT:
		break;
	case MenuType::CHOOSE_CHARACTER_CLASS:
		break;
	case MenuType::CHOOSE_CHARACTER_PORTRAIT:
		break;
	case MenuType::CHOOSE_CHARACTER_RACE:
		break;
	case MenuType::OPTIONS:
		_add_item(0, MenuItemType::ENTRY, MenuItem::OP_RECOMMENDED_MODE, (*_display.string)["CONFIG_RECOMMENDED_MODE"],
		true, ConfigOption::RECOMMENDED_MODE, (*_display.string)["HINT_CONFIG_RECOMMENDED_MODE"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::OP_STRICT_MODE, (*_display.string)["CONFIG_STRICT_MODE"], true,
			ConfigOption::STRICT_MODE, (*_display.string)["HINT_CONFIG_STRICT_MODE"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::OP_CHEAT_MODE, (*_display.string)["CONFIG_CHEAT_MODE"],  true,
			ConfigOption::CHEAT_MODE, (*_display.string)["HINT_CONFIG_CHEAT_MODE"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::OP_AUTO_SAVE, (*_display.string)["CONFIG_AUTO_SAVE"], true,
			ConfigOption::AUTO_SAVE, (*_display.string)["HINT_CONFIG_AUTO_SAVE"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::OP_DICE_ROLLS, (*_display.string)["CONFIG_DICE_ROLLS"], true,
			ConfigOption::DICE_ROLLS, (*_display.string)["HINT_CONFIG_DICE_ROLLS"]);
		_add_item(5, MenuItemType::ENTRY, MenuItem::OP_ALLOW_MIXED_ALIGNMENT_PARTY,
			(*_display.string)["GAME_ALLOW_MIXED_ALIGNMENT"], true, ConfigOption::ALLOW_MIXED_ALIGNMENT_PARTY,
			(*_display.string)["HINT_GAME_ALLOW_MIXED_ALIGNMENT"]);
		_add_item(6, MenuItemType::ENTRY, MenuItem::OP_STAT_LOSS_ON_LEVEL_UP,
			(*_display.string)["GAME_STAT_LOSS_LEVEL_GAIN"], true, ConfigOption::STAT_LOSS_ON_LEVEL_UP,
			(*_display.string)["HINT_GAME_STAT_LOSS_LEVEL_GAIN"]);
		_add_item(7, MenuItemType::ENTRY, MenuItem::OP_REROLL_HIT_POINTS_ON_LEVEL_GAIN,
			(*_display.string)["GAME_REROLL_HIT_POINTS"], true, ConfigOption::REROLL_HIT_POINTS_ON_LEVEL_GAIN,
			(*_display.string)["HINT_GAME_REROLL_HIT_POINTS"]);
		_add_item(8, MenuItemType::ENTRY, MenuItem::OP_STAT_RESET_ON_CLASS_CHANGE,
			(*_display.string)["GAME_STAT_RESET_CLASS_CHANGE"], true, ConfigOption::STAT_RESET_ON_CLASS_CHANGE,
			(*_display.string)["HINT_GAME_STAT_RESET_CLASS_CHANGE"]);
		_add_item(9, MenuItemType::ENTRY, MenuItem::OP_AGING_ON_CLASS_CHANGE,
			(*_display.string)["GAME_AGING_CLASS_CHANGE"], true, ConfigOption::AGING_ON_CLASS_CHANGE,
			(*_display.string)["HINT_GAME_AGING_CLASS_CHANGE"]);
		_add_item(10, MenuItemType::ENTRY, MenuItem::OP_ALLOW_AMBUSH_HIDE, (*_display.string)["GAME_ALLOW_AMBUSH_HIDE"],
			true, ConfigOption::ALLOW_AMBUSH_HIDE, (*_display.string)["HINT_GAME_ALLOW_AMBUSH_HIDE"]);
		_add_item(11, MenuItemType::ENTRY, MenuItem::OP_ALLOW_RANGED_WEAPONS,
			(*_display.string)["GAME_ALLOW_RANGED_WEAPONS"], true, ConfigOption::ALLOW_RANGED_WEAPONS,
			(*_display.string)["HINT_GAME_ALLOW_RANGED_WEAPONS"]);
		_add_item(12, MenuItemType::ENTRY, MenuItem::OP_SPELLCASTING_IN_SURPRISE_ROUND,
			(*_display.string)["GAME_SPELL_CASTING_SURPRISE_ROUND"], true, ConfigOption::SPELLCASTING_IN_SURPRISE_ROUND,
			(*_display.string)["HINT_GAME_SPELL_CASTING_SURPRISE_ROUND"]);
		_add_item(13, MenuItemType::ENTRY, MenuItem::OP_BATCH_HEALING_AFTER_RETURN_TO_CASTLE,
			(*_display.string)["GAME_BATCH_HEALING_AFTER_RETURN"], true,
			ConfigOption::BATCH_HEALING_AFTER_RETURN_TO_CASTLE,
			(*_display.string)["HINT_GAME_BATCH_HEALING_AFTER_RETURN"]);
		_add_item(14, MenuItemType::ENTRY, MenuItem::OP_REROLL_ONES_ON_DICE, (*_display.string)["GAME_REROLL_ONES"],
			true, ConfigOption::REROLL_ONES_ON_DICE, (*_display.string)["HINT_GAME_REROLL_ONES"]);
		_add_item(15, MenuItemType::SPACER, MenuItem::SPACER, (*_display.string)["MENU_SPACER"]);
		_add_item(16, MenuItemType::SPACER, MenuItem::SPACER, (*_display.string)["MENU_SPACER"]);
		_add_item(17, MenuItemType::ENTRY, MenuItem::OP_WIREFRAME_MODE, (*_display.string)["GRAPHICS_WIREFRAME"], true,
			ConfigOption::WIREFRAME_MODE, (*_display.string)["HINT_GRAPHICS_WIREFRAME"]);
		_add_item(18, MenuItemType::ENTRY, MenuItem::OP_DISPLAY_TEXTURES, (*_display.string)["GRAPHICS_TEXTURES"], true,
			ConfigOption::DISPLAY_TEXTURES, (*_display.string)["HINT_GRAPHICS_TEXTURES"]);
		_add_item(19, MenuItemType::ENTRY, MenuItem::OP_DISPLAY_TRAPS, (*_display.string)["GRAPHICS_TRAPS"], true,
			ConfigOption::DISPLAY_TRAPS, (*_display.string)["HINT_GRAPHICS_TRAPS"]);
		_add_item(20, MenuItemType::ENTRY, MenuItem::OP_DISPLAY_TELEPORTERS, (*_display.string)["GRAPHICS_TELEPORTERS"],
			true, ConfigOption::DISPLAY_TELEPORTERS, (*_display.string)["HINT_GRAPHICS_TELEPORTERS"]);
		_add_item(21, MenuItemType::ENTRY, MenuItem::OP_DISPLAY_ENCOUNTERS, (*_display.string)["GRAPHICS_ENCOUNTERS"],
			true, ConfigOption::DISPLAY_ENCOUNTERS, (*_display.string)["HINT_GRAPHICS_ENCOUNTERS"]);
		_add_item(22, MenuItemType::ENTRY, MenuItem::OP_DISPLAY_PROGRESS, (*_display.string)["GRAPHICS_PROGRESS"], true,
			ConfigOption::DISPLAY_PROGRESS, (*_display.string)["HINT_GRAPHICS_PROGRESS"]);
		_add_item(23, MenuItemType::SPACER, MenuItem::SPACER, (*_display.string)["MENU_SPACER"]);
		_add_item(23, MenuItemType::SAVE, MenuItem::SAVE, (*_display.string)["MENU_OPTIONS_SAVE"]);
		_add_item(24, MenuItemType::CANCEL, MenuItem::CANCEL, (*_display.string)["MENU_OPTIONS_CANCEL"]);
		selected = items.begin();
		break;
	case MenuType::PAUSE:
		break;
	case MenuType::REVIEW_AND_CONFIRM:
		break;
	default:
		break;
	}
}

// Overload [] Operator
auto Sorcery::Menu::operator [] (const unsigned int index) -> const MenuEntry& {
	return items.at(index);
}

auto Sorcery::Menu::get_type() -> MenuType {
	return _type;
}

// Add an item to the Menu
auto Sorcery::Menu::_add_item(int index, const MenuItemType itemtype, const MenuItem code, std::string& key) -> void {
	if (key.length() % 2 == 0)
		key.resize(key.length() + 1, 32);

	std::string hint {};
	items.push_back({static_cast<unsigned int>(index), itemtype, code, key, true, ConfigOption::NONE, hint});
	++count;
}

// Add an item to the Menu
auto Sorcery::Menu::_add_item(int index, const MenuItemType itemtype, const MenuItem code, std::string& key,
	bool enabled, ConfigOption option, std::string& hint) -> void {
	if (key.length() % 2 == 0)
		key.resize(key.length() + 1, 32);

	items.push_back({static_cast<unsigned int>(index), itemtype, code, key, enabled, option, hint});
	++count;
}

// Select the first enabled menu item
auto Sorcery::Menu::_select_first_enabled() -> std::optional<std::vector<MenuEntry>::const_iterator> {
	for (std::vector<MenuEntry>::const_iterator it = items.begin(); it != items.end(); ++it)
		if ((((*it).type == MenuItemType::ENTRY) || ((*it).type == MenuItemType::SAVE) ||
			((*it).type == MenuItemType::CANCEL)) && ((*it).enabled)) {
			selected = it;
			return selected;
		}

	return std::nullopt;
}

// Select the last enabled menu item
auto Sorcery::Menu::_select_last_enabled() -> std::optional<std::vector<MenuEntry>::const_iterator> {
	for (std::vector<MenuEntry>::const_iterator it = items.end() - 1; it != items.begin(); --it)
		if ((((*it).type == MenuItemType::ENTRY) || ((*it).type == MenuItemType::SAVE) ||
			((*it).type == MenuItemType::CANCEL)) && ((*it).enabled)) {
			selected = it;
			return selected;
		}

	return std::nullopt;
}

// Check if the mouse cursor is on a menu item
auto Sorcery::Menu::check_menu_mouseover(sf::Vector2f mouse_position) ->
	std::optional<std::vector<MenuEntry>::const_iterator> {
	bool found {false};
	std::vector<sf::FloatRect>::const_iterator working_bounds {bounds.begin()};
	std::vector<MenuEntry>::const_iterator working_items {items.begin()};
	do {
		if (working_bounds->contains(mouse_position))
			return working_items;

		++working_bounds;
		++working_items;
		} while ((working_bounds < bounds.end()) && (!found));

	// If we reach here the mouse cursor is outside the items so we don't do anything
	return std::nullopt;
}

// Check if the mouse cursor is on a menu item, and if so set it
auto Sorcery::Menu::set_mouse_selected(sf::Vector2f mouse_position) ->
	std::optional<std::vector<MenuEntry>::const_iterator> {

	if (bounds.size() > 0) {
		bool found {false};
		std::vector<sf::FloatRect>::const_iterator working_bounds {bounds.begin()};
		std::vector<MenuEntry>::const_iterator working_items {items.begin()};
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
		return std::nullopt;
	}

	// And if we reach here it means that bounds (which requites a draw to take place, hasn't been populated yet)
	return std::nullopt;
}

// Set selected based upon the item index
auto Sorcery::Menu::choose(unsigned int index) -> std::optional<std::vector<MenuEntry>::const_iterator> {

	// Iterate through til we have found it
	bool found {false};
	if (index < items.size()) {
		std::vector<MenuEntry>::const_iterator working {items.begin()};
		do {
			found = (*working).index == index;
			++working;
		} while ((working > items.begin()) && (!found));
		if (found) {
			selected = working;
			return selected;
		}
	}

	// If we reach here the mouse cursor is outside the items so we don't do anything
	return std::nullopt;
}

auto Sorcery::Menu::choose_first() -> std::optional<std::vector<MenuEntry>::const_iterator> {
	return  _select_first_enabled();
}
auto Sorcery::Menu::choose_last() -> std::optional<std::vector<MenuEntry>::const_iterator> {
	return  _select_last_enabled();
}

// Choose the previous selected item
auto Sorcery::Menu::choose_previous() -> std::optional<std::vector<MenuEntry>::const_iterator> {
	if (selected > items.begin()) {

		// Iterate backwards until we find the first previous enabled menu if we can
		bool found_enabled_option {false};
		std::vector<MenuEntry>::const_iterator working {selected};
		do {
			--working;
			found_enabled_option = ((*working).enabled) && ((*working).type == MenuItemType::ENTRY);
		} while ((working >= items.begin()) && (!found_enabled_option));
		if (found_enabled_option) {
			selected = working;
			return selected;
		}
	} else
		return selected;

	return std::nullopt;
}

// Choose the next selected item
auto Sorcery::Menu::choose_next() -> std::optional<std::vector<MenuEntry>::const_iterator> {
	if (selected < (items.end() - 1)) {

		// Iterate forwards until we find the first next enabled menu if we can
		bool found_enabled_option {false};
		std::vector<MenuEntry>::const_iterator working {selected};
		do {
			++working;
			found_enabled_option = ((*working).enabled) && ((*working).type == MenuItemType::ENTRY);
		} while ((working <= items.end() - 1) && (!found_enabled_option));
		if (found_enabled_option) {
			selected = working;
			return selected;
		}
	} else
		return selected;

	return std::nullopt;
}
