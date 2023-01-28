// Copyright (C) 2023 Dave Moore
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

#include "options.hpp"

// Standard Constructor
Sorcery::Options::Options(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Menu and Options
	_menu = std::make_unique<Menu>(_system, _display, _graphics, nullptr, MenuType::OPTIONS);
	_menu->generate((*_display->layout)["options:menu"]);
	_menu->setPosition(_display->get_centre_x(_menu->get_width()), (*_display->layout)["options:menu"].y);

	_option_on = Component((*_display->layout)["options:on"]);
	_option_off = Component((*_display->layout)["options:off"]);

	// Get the Infopanel
	_ip = std::make_unique<InfoPanel>(_system, _display, _graphics);

	// Create the Confirmation Dialog Boxes
	_confirm_save =
		std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["options:dialog_confirm_save"],
			(*_display->layout)["options:dialog_confirm_save_text"], WindowDialogType::CONFIRM);
	_confirm_save->setPosition(_display->get_centre_pos(_confirm_save->get_size()));

	_confirm_cancel =
		std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["options:dialog_confirm_cancel"],
			(*_display->layout)["options:dialog_confirm_cancel_text"], WindowDialogType::CONFIRM);
	_confirm_cancel->setPosition(_display->get_centre_pos(_confirm_cancel->get_size()));

	_confirm_strict =
		std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["options:dialog_confirm_strict_on"],
			(*_display->layout)["options:dialog_confirm_strict_on_text"], WindowDialogType::CONFIRM);
	_confirm_strict->setPosition(_display->get_centre_pos(_confirm_strict->get_size()));
}

// Standard Destructor
Sorcery::Options::~Options() {

	_display->stop_bg_movie();
}

auto Sorcery::Options::start() -> int {

	// Get the Background Display Components and load them into Display module
	// storage (not local)
	_display->generate("options");

	// On entry store the current settings
	_system->config->store();

	// Clear the window
	_window->clear();
	_ip->valid = false;

	// Play the background movie!
	_display->fit_bg_movie();
	_display->start_bg_movie();

	// And select the first option by default;
	_display->set_input_mode(WindowInputMode::GAME_OPTIONS);
	_menu->choose_first();
	std::optional<std::vector<MenuEntry>::const_iterator> selected{_menu->items.begin()};
	_set_infopanel(_menu->selected);

	// And do the main loop
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// If we are in normal input mode
			if (_display->get_input_mode() == WindowInputMode::GAME_OPTIONS) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return EXIT_ALL;

				if (_system->input->check(WindowInput::CANCEL, event))
					return EXIT_MODULE;

				// Handle enabling help overlay
				if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				// And handle input on the main menu
				if (_system->input->check(WindowInput::UP, event)) {
					selected = _menu->choose_previous();
				} else if (_system->input->check(WindowInput::DOWN, event)) {
					selected = _menu->choose_next();
				} else if (_system->input->check(WindowInput::MOVE, event)) {
					selected = _menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					if (selected) {
						// No Tooltips anymore
					}

				} else if (_system->input->check(WindowInput::CONFIRM, event)) {
					if (selected) {
						if ((*_menu->selected).type == MenuItemType::ENTRY) {
							const ConfigOption config_to_toggle{(*_menu->selected).config};
							if ((config_to_toggle == ConfigOption::STRICT_MODE) &&
								(!(*_system->config)[ConfigOption::STRICT_MODE])) {

								// Ask for confirmation of Strict Mode
								_display->set_input_mode(WindowInputMode::CONFIRM_STRICT_MODE);

							} else if ((config_to_toggle == ConfigOption::RECOMMENDED_MODE) &&
									   (!(*_system->config)[ConfigOption::RECOMMENDED_MODE])) {

								// Handle Recommended Toggling
								_system->config->set_rec_mode();
								(*_system->config)[ConfigOption::RECOMMENDED_MODE] = true;

							} else {

								(*_system->config)[config_to_toggle] = !(*_system->config)[config_to_toggle];

								// And toggling off strict mode
								if (!_system->config->is_strict_mode())
									(*_system->config)[ConfigOption::STRICT_MODE] = false;

								if (!_system->config->is_rec_mode())
									(*_system->config)[ConfigOption::RECOMMENDED_MODE] = false;
							}
						} else if ((*_menu->selected).type == MenuItemType::SAVE) {

							// Ask for confirmation of Save
							_display->set_input_mode(WindowInputMode::SAVE_CHANGES);
						} else if ((*_menu->selected).type == MenuItemType::CANCEL) {

							// Ask for confirmation of Cancel
							_display->set_input_mode(WindowInputMode::CANCEL_CHANGES);
						}
					}
				}

				_set_infopanel(_menu->selected);

			} else if ((_display->get_input_mode() == WindowInputMode::CONFIRM_STRICT_MODE) ||
					   (_display->get_input_mode() == WindowInputMode::SAVE_CHANGES) ||
					   (_display->get_input_mode() == WindowInputMode::CANCEL_CHANGES)) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return EXIT_ALL;

				if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				// All we can do is select Y or N
				if (_display->get_input_mode() == WindowInputMode::CONFIRM_STRICT_MODE) {
					auto dialog_input{_confirm_strict->handle_input(event)};
					if (dialog_input) {
						if (dialog_input.value() == WindowDialogButton::CLOSE) {
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							return EXIT_MODULE;
						} else if (dialog_input.value() == WindowDialogButton::YES) {
							_system->config->set_strict_mode();
							(*_system->config)[ConfigOption::STRICT_MODE] = true;
							_display->set_input_mode(WindowInputMode::GAME_OPTIONS);
						} else if (dialog_input.value() == WindowDialogButton::NO) {
							_display->set_input_mode(WindowInputMode::GAME_OPTIONS);
						}
					}
				} else if (_display->get_input_mode() == WindowInputMode::SAVE_CHANGES) {
					auto dialog_input{_confirm_save->handle_input(event)};
					if (dialog_input) {
						if (dialog_input.value() == WindowDialogButton::CLOSE) {
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							return EXIT_MODULE;
						} else if (dialog_input.value() == WindowDialogButton::YES) {
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							_system->config->save();
							return true;
						} else if (dialog_input.value() == WindowDialogButton::NO) {
							_display->set_input_mode(WindowInputMode::GAME_OPTIONS);
						}
					}
				} else if (_display->get_input_mode() == WindowInputMode::CANCEL_CHANGES) {

					auto dialog_input{_confirm_cancel->handle_input(event)};
					if (dialog_input) {
						if (dialog_input.value() == WindowDialogButton::CLOSE) {
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							return false;
						} else if (dialog_input.value() == WindowDialogButton::YES) {
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							_system->config->load();
							return true;
						} else if (dialog_input.value() == WindowDialogButton::NO) {
							_display->set_input_mode(WindowInputMode::GAME_OPTIONS);
						}
					}
				}
			}
		}

		_window->clear();

		_display->start_bg_movie();
		_display->update_bg_movie();
		_display->draw_bg_movie();

		_draw();
		_window->display();
	}

	return false;
}

auto Sorcery::Options::stop() -> void {

	_display->stop_bg_movie();
}

auto Sorcery::Options::_set_infopanel(std::vector<Sorcery::MenuEntry>::const_iterator it) -> void {

	// Set the Text
	if ((*it).type == MenuItemType::ENTRY) {
		auto ip_contents{(*it).hint};
		_ip->set_text(ip_contents);
		_ip->valid = true;
	} else
		_ip->valid = false;

	// Set the Icon
	if ((*it).type == MenuItemType::ENTRY)
		_ip->set_icon(MenuItem::OP_ALL);
}

auto Sorcery::Options::_draw() -> void {

	_display->display("options");

	_menu->generate((*_display->layout)["options:menu"]);
	_window->draw(*_menu);
	if (_display->get_input_mode() == WindowInputMode::CONFIRM_STRICT_MODE) {
		_confirm_strict->update();
		_window->draw(*_confirm_strict);
	} else if (_display->get_input_mode() == WindowInputMode::SAVE_CHANGES) {
		_confirm_save->update();
		_window->draw(*_confirm_save);
	} else if (_display->get_input_mode() == WindowInputMode::CANCEL_CHANGES) {
		_confirm_cancel->update();
		_window->draw(*_confirm_cancel);
	}

	if (_ip->valid) {
		_ip->setPosition((*_display->layout)["options:info_panel"].x, (*_display->layout)["options:info_panel"].y);
		_window->draw(*_ip);
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}