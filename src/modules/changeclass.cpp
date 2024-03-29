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

#include "modules/changeclass.hpp"
#include "common/enum.hpp"
#include "core/display.hpp"
#include "core/graphics.hpp"
#include "core/input.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "gui/dialog.hpp"
#include "gui/frame.hpp"
#include "gui/infopanel.hpp"
#include "gui/menu.hpp"
#include "resources/componentstore.hpp"
#include "types/character.hpp"
#include "types/component.hpp"

// Standard Constructor
Sorcery::ChangeClass::ChangeClass(System *system, Display *display, Graphics *graphics, Character *character)
	: _system{system}, _display{display}, _graphics{graphics}, _character{character} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Get the Infopanel
	_ip = std::make_unique<InfoPanel>(_system, _display, _graphics);

	// Main Menu
	_menu = std::make_unique<Menu>(_system, _display, _graphics, nullptr, MenuType::CHANGE_CHARACTER_CLASS);
	_set_classes_menu();
	_menu->choose_first();
	_menu->generate((*_display->layout)["change_class:menu"]);
	_menu->setPosition(_display->get_centre_x(_menu->get_width()), (*_display->layout)["change_class:menu"].y);

	;

	// Info Panel
	_ip->valid = false;
	_set_info_panel_contents(_menu->selected);

	// And the Dialogs
	_not_changed = std::make_unique<Dialog>(_system, _display, _graphics,
		(*_display->layout)["change_class:dialog_class_not_changed"],
		(*_display->layout)["change_class:dialog_class_not_changed_text"], WindowDialogType::OK);
	_not_changed->setPosition(_display->get_centre_pos(_not_changed->get_size()));

	_confirm = std::make_unique<Dialog>(_system, _display, _graphics,
		(*_display->layout)["change_class:dialog_confirm_change_class"],
		(*_display->layout)["change_class:dialog_confirm_change_class_text"], WindowDialogType::CONFIRM);
	_confirm->setPosition(_display->get_centre_pos(_confirm->get_size()));

	_new_class = std::nullopt;
}

// Standard Destructor
Sorcery::ChangeClass::~ChangeClass() {
}

auto Sorcery::ChangeClass::start() -> std::optional<CharacterClass> {

	_show_confirm = false;
	_show_not_changed = false;

	_display->generate("change_class");
	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);

	// Clear the window
	_window->clear();

	while (_window->isOpen()) {
		sf::Event event{};
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				return std::nullopt; // TODO: how to differ from cancel?

			// Handle enabling help overlay
			if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			if (_show_not_changed) {

				auto dialog_input{_not_changed->handle_input(event)};
				if (dialog_input) {
					if (dialog_input.value() == WindowDialogButton::CLOSE) {
						_show_not_changed = false;
						_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
						return std::nullopt;
					} else if (dialog_input.value() == WindowDialogButton::OK) {
						_show_not_changed = false;
						_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
						return std::nullopt;
					}
				};
			} else if (_show_confirm) {

				auto dialog_input{_confirm->handle_input(event)};
				if (dialog_input) {
					if (dialog_input.value() == WindowDialogButton::CLOSE) {
						_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
						_show_confirm = false;
					} else if (dialog_input.value() == WindowDialogButton::YES) {
						_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
						return _new_class;
					} else if (dialog_input.value() == WindowDialogButton::NO) {
						_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
						_show_confirm = false;
					}
				}

			} else {

				if (_system->input->check(WindowInput::CANCEL, event))
					return std::nullopt;

				if (_system->input->check(WindowInput::BACK, event))
					return std::nullopt;

				std::optional<std::vector<MenuEntry>::const_iterator> selected{_menu->selected};
				if (_system->input->check(WindowInput::UP, event))
					selected = _menu->choose_previous();
				else if (_system->input->check(WindowInput::DOWN, event))
					selected = _menu->choose_next();
				else if (_system->input->check(WindowInput::MOVE, event))
					selected = _menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
				else if (_system->input->check(WindowInput::BACK, event))
					return std::nullopt;
				else if (_system->input->check(WindowInput::DELETE, event))
					return std::nullopt;
				else if (_system->input->check(WindowInput::CONFIRM, event)) {

					// We have selected something from the menu
					if (selected) {

						switch ((*selected.value()).item) {
						case MenuItem::CC_SAMURAI:
							if (_character->get_class() == CharacterClass::SAMURAI)
								_show_not_changed = true;
							else {
								_show_confirm = true;
								_new_class = CharacterClass::SAMURAI;
								_display->set_input_mode(WindowInputMode::CONFIRM_CHANGE_CHARACTER_CLASS);
							}
							break;
						case MenuItem::CC_FIGHTER:
							if (_character->get_class() == CharacterClass::FIGHTER)
								_show_not_changed = true;
							else {
								_show_confirm = true;
								_new_class = CharacterClass::FIGHTER;
								_display->set_input_mode(WindowInputMode::CONFIRM_CHANGE_CHARACTER_CLASS);
							}
							break;
						case MenuItem::CC_LORD:
							if (_character->get_class() == CharacterClass::LORD)
								_show_not_changed = true;
							else {
								_show_confirm = true;
								_new_class = CharacterClass::LORD;
								_display->set_input_mode(WindowInputMode::CONFIRM_CHANGE_CHARACTER_CLASS);
							}
							break;
						case MenuItem::CC_THIEF:
							if (_character->get_class() == CharacterClass::THIEF)
								_show_not_changed = true;
							else {
								_show_confirm = true;
								_new_class = CharacterClass::THIEF;
								_display->set_input_mode(WindowInputMode::CONFIRM_CHANGE_CHARACTER_CLASS);
							}
							break;
						case MenuItem::CC_NINJA:
							if (_character->get_class() == CharacterClass::NINJA)
								_show_not_changed = true;
							else {
								_show_confirm = true;
								_new_class = CharacterClass::NINJA;
								_display->set_input_mode(WindowInputMode::CONFIRM_CHANGE_CHARACTER_CLASS);
							}
							break;
						case MenuItem::CC_PRIEST:
							if (_character->get_class() == CharacterClass::PRIEST)
								_show_not_changed = true;
							else {
								_show_confirm = true;
								_new_class = CharacterClass::PRIEST;
								_display->set_input_mode(WindowInputMode::CONFIRM_CHANGE_CHARACTER_CLASS);
							}
							break;
						case MenuItem::CC_BISHOP:
							if (_character->get_class() == CharacterClass::BISHOP)
								_show_not_changed = true;
							else {
								_show_confirm = true;
								_new_class = CharacterClass::BISHOP;
								_display->set_input_mode(WindowInputMode::CONFIRM_CHANGE_CHARACTER_CLASS);
							}
							break;
						case MenuItem::CC_MAGE:
							if (_character->get_class() == CharacterClass::MAGE)
								_show_not_changed = true;
							else {
								_show_confirm = true;
								_new_class = CharacterClass::MAGE;
								_display->set_input_mode(WindowInputMode::CONFIRM_CHANGE_CHARACTER_CLASS);
							}
							break;
						default:
							return std::nullopt;
							break;
						}
					}
				}
			}

			_set_info_panel_contents(_menu->selected);
		}

		_window->clear();

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

	// Background Wallpaper
	_graphics->tile_bg(_window);

	// Display Components
	_display->display("change_class");

	// And the Menu
	_menu->generate((*_display->layout)["change_class:menu"]);
	_window->draw(*_menu);

	if (_ip->valid) {
		_ip->setPosition((*_display->layout)["change_class:info_panel"].pos());
		_window->draw(*_ip);
	}

	if (_show_not_changed) {
		_not_changed->update();
		_window->draw(*_not_changed);
	}

	if (_show_confirm) {
		_confirm->update();
		_window->draw(*_confirm);
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

auto Sorcery::ChangeClass::_set_info_panel_contents(std::vector<Sorcery::MenuEntry>::const_iterator it) -> void {

	// Set the Text
	if ((*it).type == MenuItemType::ENTRY) {
		const auto ip_contents{(*it).hint};
		_ip->set_text(ip_contents);
		_ip->valid = true;
	} else
		_ip->valid = false;

	// Set the Icon
	if ((*it).type == MenuItemType::ENTRY)
		_ip->set_icon((*it).item);
}
