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

#include "castle.hpp"

// Standard Constructor
Sorcery::Castle::Castle(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_menu = std::make_shared<Menu>(_system, _display, _graphics, MenuType::CASTLE);

	_dialog_leave_game = std::make_shared<Dialog>(_system, _display, _graphics,
		(*_display->layout)["castle:dialog_leave_game"],
		(*_display->layout)["castle:dialog_leave_game_text"], WindowDialogType::CONFIRM);
	_dialog_leave_game->setPosition((*_display->layout)["castle:dialog_leave_game"].x,
		(*_display->layout)["castle:dialog_leave_game"].y);

	// Modules
	_status_bar = std::make_unique<StatusBar>(_system, _display, _graphics);
	_edge_of_town = std::make_shared<EdgeOfTown>(system, display, graphics);
}

// Standard Destructor
Sorcery::Castle::~Castle() {}

// Start/Continue a new Game
auto Sorcery::Castle::start(bool new_game) -> std::optional<MenuItem> {

	if (new_game)
		_system->database->add_game();

	// Get the Background Display Components and load them into Display module storage (not local -
	// and note that due to the way both menus are combined in this class, we need to have the
	// menustage set first in this case and this case only)
	_display->generate_components("castle");

	// Clear the window
	_window->clear();

	// Generate the Components
	const Component status_bar_c{(*_display->layout)["status_bar:status_bar"]};
	_status_bar->setPosition(_display->window->get_x(_status_bar->sprite, status_bar_c.x),
		_display->window->get_y(_status_bar->sprite, status_bar_c.y));

	// And do the main loop
	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
	std::optional<std::vector<MenuEntry>::const_iterator> option{_menu->items.begin()};
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// If we are in normal input mode
			if (_display->get_input_mode() == WindowInputMode::NAVIGATE_MENU) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					_window->close();

				// Handle enabling help overlay
				if (_system->input->check_for_event(WindowInput::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				// And handle input on the main menu
				if (_system->input->check_for_event(WindowInput::UP, event))
					option = _menu->choose_previous();
				else if (_system->input->check_for_event(WindowInput::DOWN, event))
					option = _menu->choose_next();
				else if (_system->input->check_for_event(WindowInput::MOVE, event))
					option = _menu->set_mouse_selected(
						static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
				else if (_system->input->check_for_event(WindowInput::CONFIRM, event)) {

					// We have selected something from the menu
					if (option) {
						if (const MenuItem option_chosen{(*option.value()).item};
							option_chosen == MenuItem::CA_EDGE_OF_TOWN) {
							auto edge_option{_edge_of_town->start()};
							_edge_of_town->stop();
							if (edge_option) {
								if (edge_option.value() == MenuItem::ET_LEAVE_GAME)
									return MenuItem::ET_LEAVE_GAME;
							}
							_display->generate_components("castle");
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							continue;
						}
					}
				} else if ((_system->input->check_for_event(WindowInput::CANCEL, event)) ||
						   ((_system->input->check_for_event(WindowInput::BACK, event)))) {
					_display->set_input_mode(WindowInputMode::CONFIRM_LEAVE_GAME);
					_yes_or_no = WindowConfirm::NO;
				}
			} else if (_display->get_input_mode() == WindowInputMode::CONFIRM_LEAVE_GAME) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					_window->close();

				if (_system->input->check_for_event(WindowInput::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				// All we can do is select Y or N
				if (_system->input->check_for_event(WindowInput::LEFT, event))
					_dialog_leave_game->toggle_highlighted();
				else if (_system->input->check_for_event(WindowInput::RIGHT, event))
					_dialog_leave_game->toggle_highlighted();
				else if (_system->input->check_for_event(WindowInput::YES, event))
					_dialog_leave_game->set_selected(WindowDialogButton::YES);
				else if (_system->input->check_for_event(WindowInput::NO, event))
					_dialog_leave_game->set_selected(WindowDialogButton::NO);
				else if (_system->input->check_for_event(WindowInput::CANCEL, event))
					_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
				else if (_system->input->check_for_event(WindowInput::BACK, event))
					_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
				else if (_system->input->check_for_event(WindowInput::MOVE, event))
					_dialog_leave_game->check_for_mouse_move(
						static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
				else if (_system->input->check_for_event(WindowInput::CONFIRM, event)) {
					std::optional<WindowDialogButton> button_chosen{
						_dialog_leave_game->check_if_option_selected(
							static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)))};

					// Mouse click only
					if (button_chosen) {
						if (button_chosen.value() == WindowDialogButton::YES) {
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							return MenuItem::ET_LEAVE_GAME;
						} else if (button_chosen.value() == WindowDialogButton::NO)
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);

					} else {

						// Button/Keyboard
						if (_dialog_leave_game->get_selected() == WindowDialogButton::YES) {
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							return MenuItem::ET_LEAVE_GAME;
						} else if (_dialog_leave_game->get_selected() == WindowDialogButton::NO)
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
					}
				}
			}
		}

		_window->clear();

		_draw();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::Castle::stop() -> void {}

auto Sorcery::Castle::_draw() -> void {

	// Custom Components
	_display->display_components("castle");
	_window->draw(*_status_bar);

	// And the Menu
	const double lerp{_graphics->animation->colour_lerp};
	_menu->generate((*_display->layout)["castle:menu"], lerp);
	const sf::Vector2f menu_pos(
		(*_display->layout)["castle:menu"].x, (*_display->layout)["castle:menu"].y);
	_menu->setPosition(menu_pos);
	_window->draw(*_menu);
	if (_display->get_input_mode() == WindowInputMode::CONFIRM_LEAVE_GAME) {
		_dialog_leave_game->update();
		_window->draw(*_dialog_leave_game);
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}
