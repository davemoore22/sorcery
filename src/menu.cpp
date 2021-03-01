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

#include "menu.hpp"

// Standard Constructor
Sorcery::Menu::Menu(System &system, Display &display, Graphics &graphics, const MenuType type)
	: _system{system}, _display{display}, _graphics{graphics}, _type{type} {

	// Clear the Items
	items.clear();
	bounds.clear();
	count = 0;

	// Now depending on the menu type, add the relevant items
	switch (_type) {
	case MenuType::MAIN:
		_add_item(0, MenuItemType::ENTRY, MenuItem::MM_NEW_GAME,
			(*_display.string)["MAIN_MENU_OPTION_START"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::MM_CONTINUE_GAME,
			(*_display.string)["MAIN_MENU_OPTION_CONTINUE"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::MM_OPTIONS,
			(*_display.string)["MAIN_MENU_OPTION_OPTIONS"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::MM_COMPENDIUM,
			(*_display.string)["MAIN_MENU_OPTION_COMPENDIUM"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::MM_LICENSE,
			(*_display.string)["MAIN_MENU_OPTION_LICENSE"]);
		_add_item(
			5, MenuItemType::ENTRY, MenuItem::QUIT, (*_display.string)["MAIN_MENU_OPTION_EXIT"]);
		selected = items.begin();
		break;
	case MenuType::CASTLE:
		_add_item(0, MenuItemType::ENTRY, MenuItem::CA_TAVERN, (*_display.string)["MENU_TAVERN"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::CA_INN, (*_display.string)["MENU_INN"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::CA_SHOP, (*_display.string)["MENU_SHOP"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::CA_TEMPLE, (*_display.string)["MENU_TEMPLE"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::CA_EDGE_OF_TOWN,
			(*_display.string)["MENU_EDGE_OF_TOWN"]);
		selected = items.begin();
		break;
	case MenuType::EDGE_OF_TOWN:
		_add_item(0, MenuItemType::ENTRY, MenuItem::ET_CASTLE, (*_display.string)["MENU_CASTLE"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::ET_TRAIN, (*_display.string)["MENU_TRAIN"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::ET_MAZE, (*_display.string)["MENU_MAZE"]);
		_add_item(
			3, MenuItemType::ENTRY, MenuItem::ET_LEAVE_GAME, (*_display.string)["MENU_LEAVE_GAME"]);
		selected = items.begin();
		break;
	case MenuType::TRAINING_GROUNDS:
		_add_item(0, MenuItemType::ENTRY, MenuItem::TR_CREATE,
			(*_display.string)["TRAINING_GROUNDS_MENU_OPTION_CREATE"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::TR_EDIT,
			(*_display.string)["TRAINING_GROUNDS_MENU_OPTION_EDIT"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::TR_DELETE,
			(*_display.string)["TRAINING_GROUNDS_MENU_OPTION_DELETE"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::TR_INSPECT,
			(*_display.string)["TRAINING_GROUNDS_MENU_OPTION_INSPECT"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::TR_EDGE_OF_TOWN,
			(*_display.string)["TRAINING_GROUNDS_MENU_OPTION_RETURN"]);
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
		_add_item(0, MenuItemType::ENTRY, MenuItem::OP_RECOMMENDED_MODE,
			(*_display.string)["CONFIG_RECOMMENDED_MODE"], true, ConfigOption::RECOMMENDED_MODE,
			(*_display.string)["HINT_CONFIG_RECOMMENDED_MODE"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::OP_STRICT_MODE,
			(*_display.string)["CONFIG_STRICT_MODE"], true, ConfigOption::STRICT_MODE,
			(*_display.string)["HINT_CONFIG_STRICT_MODE"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::OP_CHEAT_MODE,
			(*_display.string)["CONFIG_CHEAT_MODE"], true, ConfigOption::CHEAT_MODE,
			(*_display.string)["HINT_CONFIG_CHEAT_MODE"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::OP_AUTO_SAVE,
			(*_display.string)["CONFIG_AUTO_SAVE"], true, ConfigOption::AUTO_SAVE,
			(*_display.string)["HINT_CONFIG_AUTO_SAVE"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::OP_DICE_ROLLS,
			(*_display.string)["CONFIG_DICE_ROLLS"], true, ConfigOption::DICE_ROLLS,
			(*_display.string)["HINT_CONFIG_DICE_ROLLS"]);
		_add_item(5, MenuItemType::ENTRY, MenuItem::OP_ALLOW_MIXED_ALIGNMENT_PARTY,
			(*_display.string)["GAME_ALLOW_MIXED_ALIGNMENT"], true,
			ConfigOption::ALLOW_MIXED_ALIGNMENT_PARTY,
			(*_display.string)["HINT_GAME_ALLOW_MIXED_ALIGNMENT"]);
		_add_item(6, MenuItemType::ENTRY, MenuItem::OP_STAT_LOSS_ON_LEVEL_UP,
			(*_display.string)["GAME_STAT_LOSS_LEVEL_GAIN"], true,
			ConfigOption::STAT_LOSS_ON_LEVEL_UP,
			(*_display.string)["HINT_GAME_STAT_LOSS_LEVEL_GAIN"]);
		_add_item(7, MenuItemType::ENTRY, MenuItem::OP_REROLL_HIT_POINTS_ON_LEVEL_GAIN,
			(*_display.string)["GAME_REROLL_HIT_POINTS"], true,
			ConfigOption::REROLL_HIT_POINTS_ON_LEVEL_GAIN,
			(*_display.string)["HINT_GAME_REROLL_HIT_POINTS"]);
		_add_item(8, MenuItemType::ENTRY, MenuItem::OP_STAT_RESET_ON_CLASS_CHANGE,
			(*_display.string)["GAME_STAT_RESET_CLASS_CHANGE"], true,
			ConfigOption::STAT_RESET_ON_CLASS_CHANGE,
			(*_display.string)["HINT_GAME_STAT_RESET_CLASS_CHANGE"]);
		_add_item(9, MenuItemType::ENTRY, MenuItem::OP_AGING_ON_CLASS_CHANGE,
			(*_display.string)["GAME_AGING_CLASS_CHANGE"], true,
			ConfigOption::AGING_ON_CLASS_CHANGE,
			(*_display.string)["HINT_GAME_AGING_CLASS_CHANGE"]);
		_add_item(10, MenuItemType::ENTRY, MenuItem::OP_ALLOW_AMBUSH_HIDE,
			(*_display.string)["GAME_ALLOW_AMBUSH_HIDE"], true, ConfigOption::ALLOW_AMBUSH_HIDE,
			(*_display.string)["HINT_GAME_ALLOW_AMBUSH_HIDE"]);
		_add_item(11, MenuItemType::ENTRY, MenuItem::OP_ALLOW_RANGED_WEAPONS,
			(*_display.string)["GAME_ALLOW_RANGED_WEAPONS"], true,
			ConfigOption::ALLOW_RANGED_WEAPONS,
			(*_display.string)["HINT_GAME_ALLOW_RANGED_WEAPONS"]);
		_add_item(12, MenuItemType::ENTRY, MenuItem::OP_SPELLCASTING_IN_SURPRISE_ROUND,
			(*_display.string)["GAME_SPELL_CASTING_SURPRISE_ROUND"], true,
			ConfigOption::SPELLCASTING_IN_SURPRISE_ROUND,
			(*_display.string)["HINT_GAME_SPELL_CASTING_SURPRISE_ROUND"]);
		_add_item(13, MenuItemType::ENTRY, MenuItem::OP_BATCH_HEALING_AFTER_RETURN_TO_CASTLE,
			(*_display.string)["GAME_BATCH_HEALING_AFTER_RETURN"], true,
			ConfigOption::BATCH_HEALING_AFTER_RETURN_TO_CASTLE,
			(*_display.string)["HINT_GAME_BATCH_HEALING_AFTER_RETURN"]);
		_add_item(14, MenuItemType::ENTRY, MenuItem::OP_REROLL_ONES_ON_DICE,
			(*_display.string)["GAME_REROLL_ONES"], true, ConfigOption::REROLL_ONES_ON_DICE,
			(*_display.string)["HINT_GAME_REROLL_ONES"]);
		_add_item(15, MenuItemType::SPACER, MenuItem::SPACER, (*_display.string)["MENU_SPACER"]);
		_add_item(16, MenuItemType::SPACER, MenuItem::SPACER, (*_display.string)["MENU_SPACER"]);
		_add_item(17, MenuItemType::ENTRY, MenuItem::OP_WIREFRAME_MODE,
			(*_display.string)["GRAPHICS_WIREFRAME"], true, ConfigOption::WIREFRAME_MODE,
			(*_display.string)["HINT_GRAPHICS_WIREFRAME"]);
		_add_item(18, MenuItemType::ENTRY, MenuItem::OP_DISPLAY_TEXTURES,
			(*_display.string)["GRAPHICS_TEXTURES"], true, ConfigOption::DISPLAY_TEXTURES,
			(*_display.string)["HINT_GRAPHICS_TEXTURES"]);
		_add_item(19, MenuItemType::ENTRY, MenuItem::OP_DISPLAY_TRAPS,
			(*_display.string)["GRAPHICS_TRAPS"], true, ConfigOption::DISPLAY_TRAPS,
			(*_display.string)["HINT_GRAPHICS_TRAPS"]);
		_add_item(20, MenuItemType::ENTRY, MenuItem::OP_DISPLAY_TELEPORTERS,
			(*_display.string)["GRAPHICS_TELEPORTERS"], true, ConfigOption::DISPLAY_TELEPORTERS,
			(*_display.string)["HINT_GRAPHICS_TELEPORTERS"]);
		_add_item(21, MenuItemType::ENTRY, MenuItem::OP_DISPLAY_ENCOUNTERS,
			(*_display.string)["GRAPHICS_ENCOUNTERS"], true, ConfigOption::DISPLAY_ENCOUNTERS,
			(*_display.string)["HINT_GRAPHICS_ENCOUNTERS"]);
		_add_item(22, MenuItemType::ENTRY, MenuItem::OP_DISPLAY_PROGRESS,
			(*_display.string)["GRAPHICS_PROGRESS"], true, ConfigOption::DISPLAY_PROGRESS,
			(*_display.string)["HINT_GRAPHICS_PROGRESS"]);
		_add_item(23, MenuItemType::SPACER, MenuItem::SPACER, (*_display.string)["MENU_SPACER"]);
		_add_item(24, MenuItemType::SAVE, MenuItem::SAVE, (*_display.string)["MENU_OPTIONS_SAVE"]);
		_add_item(
			25, MenuItemType::CANCEL, MenuItem::CANCEL, (*_display.string)["MENU_OPTIONS_CANCEL"]);
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
auto Sorcery::Menu::operator[](const unsigned int index) -> const MenuEntry & {

	return items.at(index);
}

auto Sorcery::Menu::get_type() -> MenuType {

	return _type;
}

// Add an item to the Menu
auto Sorcery::Menu::_add_item(
	int index, const MenuItemType itemtype, const MenuItem code, std::string &key) -> void {

	if (key.length() % 2 == 0)
		key.resize(key.length() + 1, 32);

	std::string hint{};
	items.push_back(
		{static_cast<unsigned int>(index), itemtype, code, key, true, ConfigOption::NONE, hint});
	++count;
}

// Add an item to the Menu
auto Sorcery::Menu::_add_item(const int index, const MenuItemType itemtype, const MenuItem code,
	std::string &key, const bool enabled, const ConfigOption option, const std::string &hint)
	-> void {

	if (key.length() % 2 == 0)
		key.resize(key.length() + 1, 32);

	items.push_back({static_cast<unsigned int>(index), itemtype, code, key, enabled, option, hint});
	++count;
}

// Select the first enabled menu item
auto Sorcery::Menu::_select_first_enabled()
	-> std::optional<std::vector<MenuEntry>::const_iterator> {

	for (std::vector<MenuEntry>::const_iterator it = items.begin(); it != items.end(); ++it)
		if ((((*it).type == MenuItemType::ENTRY) || ((*it).type == MenuItemType::SAVE) ||
				((*it).type == MenuItemType::CANCEL)) &&
			((*it).enabled)) {
			selected = it;
			return selected;
		}

	return std::nullopt;
}

// Select the last enabled menu item
auto Sorcery::Menu::_select_last_enabled()
	-> std::optional<std::vector<MenuEntry>::const_iterator> {

	for (std::vector<MenuEntry>::const_iterator it = items.end() - 1; it != items.begin(); --it)
		if ((((*it).type == MenuItemType::ENTRY) || ((*it).type == MenuItemType::SAVE) ||
				((*it).type == MenuItemType::CANCEL)) &&
			((*it).enabled)) {
			selected = it;
			return selected;
		}

	return std::nullopt;
}

// Check if the mouse cursor is on a menu item
auto Sorcery::Menu::check_menu_mouseover(sf::Vector2f mouse_pos)
	-> std::optional<std::vector<MenuEntry>::const_iterator> {

	if (bounds.size() > 0) {
		bool found{false};
		sf::Vector2f global_pos{this->getPosition()};
		mouse_pos -= global_pos;
		std::vector<sf::FloatRect>::const_iterator temp_bounds{bounds.begin()};
		std::vector<MenuEntry>::const_iterator temp_items{items.begin()};
		do {
			if (temp_bounds->contains(mouse_pos))
				return temp_items;

			++temp_bounds;
			++temp_items;
		} while ((temp_bounds < bounds.end()) && (!found));

		// If we reach here the mouse cursor is outside the items so we don't do anything
		return std::nullopt;
	}

	// And if we reach here it means that bounds (which requites a draw to take place, hasn't been
	// populated yet)
	return std::nullopt;
}

// Check if the mouse cursor is on a menu item, and if so set it
auto Sorcery::Menu::set_mouse_selected(sf::Vector2f mouse_pos)
	-> std::optional<std::vector<MenuEntry>::const_iterator> {

	if (bounds.size() > 0) {
		sf::Vector2f global_pos{this->getPosition()};
		mouse_pos -= global_pos;
		bool found{false};
		std::vector<sf::FloatRect>::const_iterator temp_bounds{bounds.begin()};
		std::vector<MenuEntry>::const_iterator working_items{items.begin()};
		do {
			if (temp_bounds->contains(mouse_pos)) {
				found = true; // NOLINT(clang-analyzer-deadcode.DeadStores)
				selected = working_items;
				return working_items;
			}

			++temp_bounds;
			++working_items;
		} while ((temp_bounds < bounds.end()) && (!found));

		// If we reach here the mouse cursor is outside the items so we don't do anything
		return std::nullopt;
	}

	// And if we reach here it means that bounds (which requites a draw to take place, hasn't been
	// populated yet)
	return std::nullopt;
}

// Set selected based upon the item index
auto Sorcery::Menu::choose(const unsigned int index)
	-> std::optional<std::vector<MenuEntry>::const_iterator> {

	// Iterate through til we have found it
	bool found{false};
	if (index < items.size()) {
		std::vector<MenuEntry>::const_iterator working{items.begin()};
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

	return _select_first_enabled();
}
auto Sorcery::Menu::choose_last() -> std::optional<std::vector<MenuEntry>::const_iterator> {

	return _select_last_enabled();
}

// Choose the previous selected item
auto Sorcery::Menu::choose_previous() -> std::optional<std::vector<MenuEntry>::const_iterator> {

	if (selected > items.begin()) {

		// Iterate backwards until we find the first previous enabled menu if we can
		bool found{false};
		std::vector<MenuEntry>::const_iterator working{selected};
		do {
			--working;
			found = ((*working).enabled) && ((*working).type == MenuItemType::ENTRY);
		} while ((working >= items.begin()) && (!found));
		if (found) {
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
		bool found{false};
		std::vector<MenuEntry>::const_iterator working{selected};
		do {
			++working;
			found = ((*working).enabled) && ((*working).type == MenuItemType::ENTRY);
		} while ((working <= items.end() - 1) && (!found));
		if (found) {
			selected = working;
			return selected;
		}
	} else
		return selected;

	return std::nullopt;
}

auto Sorcery::Menu::generate(Component &component, const double selected_lerp) -> void {

	int entry_x{0};
	int entry_y{0};
	int option_x{0};
	int option_y{0};
	int count{0};

	// In case we are generating the Options Menu
	const Component on_c{(*_display.layout)["options:option_on"]};
	const Component off_c{(*_display.layout)["options:option_off"]};
	const sf::Vector2f global_pos(component.x, component.y);

	// Work out total size of texture needed
	const unsigned int texture_w{component.w * _display.window->get_cell_width()};
	const unsigned int texture_h{component.h * _display.window->get_cell_height()};
	const sf::Vector2f texture_size(texture_w, texture_h);
	_rtexture.create(texture_size.x, texture_size.y);

	// Bounds are generated for each menu item to handle mouse over
	_texts.clear();
	_options.clear();
	bounds.clear();
	for (std::vector<MenuEntry>::const_iterator it = items.begin(); it != items.end(); ++it) {
		if (((*it).type == MenuItemType::TEXT) || ((*it).type == MenuItemType::ENTRY) ||
			((*it).type == MenuItemType::SAVE) || ((*it).type == MenuItemType::CANCEL)) {
			std::string text_string{(*it).key};
			sf::Text text;
			text.setFont(_system.resources->fonts[component.font]);
			text.setCharacterSize(component.size);
			text.setFillColor(sf::Color(component.colour));
			text.setString(text_string);

			// Check for alignment and set location appropriately
			entry_x = (component.justification == Justification::CENTRE) ? texture_w / 2 : 0;
			entry_y += _display.window->get_cell_height();
			text.setPosition(entry_x, entry_y);

			// If we have a selected entry, change the background colour
			if (selected == it) {
				sf::FloatRect bg_rect{text.getLocalBounds()};
				sf::RectangleShape bg(sf::Vector2f(
					component.w * _display.window->get_cell_width(), bg_rect.height + 2));
				bg.setPosition(0, entry_y);
				if (component.animated)
					bg.setFillColor(_display.window->change_colour(
						sf::Color(component.background), selected_lerp));
				else
					bg.setFillColor(sf::Color(component.background));
				text.setFillColor(sf::Color(component.colour));
				text.setOutlineColor(sf::Color(0, 0, 0));
				text.setOutlineThickness(2);

				_selected_bg = bg;
			}

			// Handle Justification
			if (_type == MenuType::OPTIONS) {
				if ((*it).type == MenuItemType::ENTRY) {
					if (component.justification == Justification::CENTRE)
						text.setOrigin(text.getLocalBounds().width / 2.0f,
							text.getLocalBounds().height / 2.0f);
					else
						text.setOrigin(0, text.getLocalBounds().height / 2.0f);
				} else if (((*it).type == MenuItemType::SAVE) ||
						   ((*it).type == MenuItemType::CANCEL)) {
					entry_x = (component.width * _display.window->get_cell_width()) / 2;
					text.setPosition(entry_x, entry_y);
					text.setOrigin(
						text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
				}
			} else {
				if (component.justification == Justification::CENTRE)
					text.setOrigin(
						text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
				else
					text.setOrigin(0, text.getLocalBounds().height / 2.0f);
			}

			_texts.emplace_back(text);

			// Now handle the mouse move/select (and tooltip generation)!
			if (((*it).type == MenuItemType::ENTRY) || ((*it).type == MenuItemType::SAVE) ||
				((*it).type == MenuItemType::CANCEL)) {
				sf::FloatRect actual_rect{text.getGlobalBounds()};
				bounds.push_back(actual_rect);
				WindowTooltipList::iterator tt_it = _display.window->tooltips.find((*it).hint);
				if (tt_it == _display.window->tooltips.end())
					_display.window->tooltips[(*it).hint] = actual_rect;
			} else {
				sf::FloatRect actual_rect;
				bounds.push_back(actual_rect);
				WindowTooltipList::iterator tt_it = _display.window->tooltips.find((*it).hint);
				if (tt_it == _display.window->tooltips.end())
					_display.window->tooltips[(*it).hint] = actual_rect;
			}

			// Add options in case of the Options Menu
			if ((_type == MenuType::OPTIONS) && ((*it).type == MenuItemType::ENTRY)) {
				option_y = entry_y;
				option_x = component.w * _display.window->get_cell_width();
				const bool option_value{(*_system.config)[(*it).config] ? true : false};
				sf::Text option_text{};
				if (option_value) {

					// On
					option_text.setFont(_system.resources->fonts[on_c.font]);
					option_text.setCharacterSize(on_c.size);
					option_text.setFillColor(sf::Color(on_c.colour));
					option_text.setString((*_display.string)[on_c.string_key]);
					sf::FloatRect bounds = option_text.getLocalBounds();
					option_text.setPosition(option_x - bounds.width, option_y);
					option_text.setOrigin(0, option_text.getLocalBounds().height / 2.0f);
				} else {

					// Off
					option_text.setFont(_system.resources->fonts[off_c.font]);
					option_text.setCharacterSize(off_c.size);
					option_text.setFillColor(sf::Color(off_c.colour));
					option_text.setString((*_display.string)[off_c.string_key]);
					sf::FloatRect bounds = option_text.getLocalBounds();
					option_text.setPosition(option_x - bounds.width, option_y);
					option_text.setOrigin(0, option_text.getLocalBounds().height / 2.0f);
				}

				if (selected == it) {
					option_text.setOutlineColor(sf::Color(0, 0, 0));
					option_text.setOutlineThickness(2);
				}

				_options.emplace_back(option_text);
			}
		} else {
			sf::FloatRect actual_rect;
			bounds.push_back(actual_rect);
			entry_y += _display.window->get_cell_height();
		}
		count++;
	}
}

auto Sorcery::Menu::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	target.draw(_selected_bg, states);
	for (auto &text : _texts)
		target.draw(text, states);

	if (_type == MenuType::OPTIONS)
		for (auto &option : _options)
			target.draw(option, states);
}
