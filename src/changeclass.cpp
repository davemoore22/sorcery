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

#include "changeclass.hpp"

// Standard Constructor
Sorcery::ChangeClass::ChangeClass(
	System *system, Display *display, Graphics *graphics, Character *character)
	: _system{system}, _display{display}, _graphics{graphics}, _character{
																   character} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Get the Infopanel
	_ip = std::make_unique<InfoPanel>(_system, _display, _graphics);

	// Main Menu
	_menu = std::make_unique<Menu>(_system, _display, _graphics, nullptr,
		MenuType::CHANGE_CHARACTER_CLASS);
	_set_classes_menu();

	// Info Panel
	_ip->valid = false;
	_set_info_panel_contents(_menu->selected);

	// And the Dialogs
	_changed = std::make_unique<Dialog>(_system, _display, _graphics,
		(*_display->layout)["change_class:dialog_class_changed"],
		(*_display->layout)["change_class:dialog_class_changed_text"],
		WindowDialogType::OK);
	_changed->setPosition(
		(*_display->layout)["change_class:dialog_class_changed"].x,
		(*_display->layout)["change_class:dialog_class_changed"].y);
	_not_changed = std::make_unique<Dialog>(_system, _display, _graphics,
		(*_display->layout)["change_class:dialog_class_not_changed"],
		(*_display->layout)["change_class:dialog_class_not_changed_text"],
		WindowDialogType::OK);
	_not_changed->setPosition(
		(*_display->layout)["change_class:dialog_class_not_changed"].x,
		(*_display->layout)["change_class:dialog_class_not_changed"].y);

	_confirm = std::make_unique<Dialog>(_system, _display, _graphics,
		(*_display->layout)["change_class:dialog_confirm_change_class"],
		(*_display->layout)["change_class:dialog_confirm_change_class_text"],
		WindowDialogType::OK);
	_not_changed->setPosition(
		(*_display->layout)["change_class:dialog_confirm_change_class"].x,
		(*_display->layout)["change_class:dialog_confirm_change_class"].y);
}

// Standard Destructor
Sorcery::ChangeClass::~ChangeClass() {}

auto Sorcery::ChangeClass::start() -> std::optional<CharacterClass> {

	_display->generate("change_class");
	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);

	// Clear the window
	_window->clear();

	// Play the background movie!
	_display->fit_bg_movie();
	_display->start_bg_movie();

	while (_window->isOpen()) {
		sf::Event event{};
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				_window->close();

			// Handle enabling help overlay
			if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			if (_system->input->check(WindowInput::CANCEL, event))
				return std::nullopt;

			if (_system->input->check(WindowInput::BACK, event))
				return std::nullopt;

			std::optional<std::vector<MenuEntry>::const_iterator> selected{
				_menu->selected};
			if (_system->input->check(WindowInput::UP, event))
				selected = _menu->choose_previous();
			else if (_system->input->check(WindowInput::DOWN, event))
				selected = _menu->choose_next();
			else if (_system->input->check(WindowInput::MOVE, event))
				selected = _menu->set_mouse_selected(static_cast<sf::Vector2f>(
					sf::Mouse::getPosition(*_window)));
			else if (_system->input->check(WindowInput::BACK, event))
				return std::nullopt;
			else if (_system->input->check(WindowInput::DELETE, event))
				return std::nullopt;
			else if (_system->input->check(WindowInput::CONFIRM, event)) {

				// We have selected something from the menu
				if (selected) {

					switch ((*selected.value()).item) {
					case MenuItem::CC_SAMURAI:
						return CharacterClass::SAMURAI;
						break;
					case MenuItem::CC_FIGHTER:
						return CharacterClass::FIGHTER;
						break;
					case MenuItem::CC_LORD:
						return CharacterClass::LORD;
						break;
					case MenuItem::CC_THIEF:
						return CharacterClass::THIEF;
						break;
					case MenuItem::CC_NINJA:
						return CharacterClass::NINJA;
						break;
					case MenuItem::CC_PRIEST:
						return CharacterClass::PRIEST;
						break;
					case MenuItem::CC_BISHOP:
						return CharacterClass::BISHOP;
						break;
					case MenuItem::CC_MAGE:
						return CharacterClass::MAGE;
						break;
					default:
						return std::nullopt;
						break;
					}
				}
			}

			_set_info_panel_contents(_menu->selected);
		}

		_window->clear();

		// Update Background Movie
		_display->start_bg_movie();
		_display->update_bg_movie();
		_display->draw_bg_movie();

		_draw();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::ChangeClass::stop() -> void {

	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);

	// Stop the background movie!
	_display->stop_bg_movie();
}

auto Sorcery::ChangeClass::_draw() -> void {

	// Display Components
	_display->display("change_class");

	// And the Menu
	_menu->generate((*_display->layout)["change_class:menu"]);
	const sf::Vector2f menu_pos((*_display->layout)["change_class:menu"].x,
		(*_display->layout)["change_class:menu"].y);
	_menu->setPosition(menu_pos);
	_window->draw(*_menu);

	if (_ip->valid) {
		_ip->setPosition((*_display->layout)["change_class:info_panel"].x,
			(*_display->layout)["change_class:info_panel"].y);
		_window->draw(*_ip);
	}

	// And finally the Cursor
	_display->display_overlay();
	_display->display_cursor();
}

auto Sorcery::ChangeClass::_set_classes_menu() -> void {

	_character->set_pos_class();
	auto possible_classes{_character->get_pos_class()};
	(*_menu)[0].enabled = possible_classes[CharacterClass::SAMURAI];
	(*_menu)[1].enabled = possible_classes[CharacterClass::FIGHTER];
	(*_menu)[2].enabled = possible_classes[CharacterClass::LORD];
	(*_menu)[3].enabled = possible_classes[CharacterClass::THIEF];
	(*_menu)[4].enabled = possible_classes[CharacterClass::NINJA];
	(*_menu)[5].enabled = possible_classes[CharacterClass::PRIEST];
	(*_menu)[6].enabled = possible_classes[CharacterClass::BISHOP];
	(*_menu)[7].enabled = possible_classes[CharacterClass::MAGE];

	// And select the current class
	_menu->choose(_character->get_class());
}

auto Sorcery::ChangeClass::_set_info_panel_contents(
	std::vector<Sorcery::MenuEntry>::const_iterator it) -> void {

	// Set the Text
	if ((*it).type == MenuItemType::ENTRY) {
		std::string ip_contents{(*it).hint};
		_ip->set_text(ip_contents);
		_ip->valid = true;
	} else
		_ip->valid = false;

	// Set the Icon
	if ((*it).type == MenuItemType::ENTRY)
		_ip->set_icon((*it).item);
}