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

#include "edgeoftown.hpp"

// Standard Constructor
Sorcery::EdgeOfTown::EdgeOfTown(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_menu = std::make_shared<Menu>(_system, _display, _graphics, MenuType::EDGE_OF_TOWN);

	_dialog_leave_game = std::make_shared<Dialog>(_system, _display, _graphics,
		(*_display->layout)["edge_of_town:dialog_leave_game"],
		(*_display->layout)["edge_of_town:dialog_leave_game_text"], WindowDialogType::CONFIRM);
	_dialog_leave_game->setPosition((*_display->layout)["edge_of_town:dialog_leave_game"].x,
		(*_display->layout)["edge_of_town:dialog_leave_game"].y);

	// Modules
	_status_bar = std::make_unique<StatusBar>(_system, _display, _graphics);
	_training = std::make_shared<Training>(_system, _display, _graphics, _game);
}

// Standard Destructor
Sorcery::EdgeOfTown::~EdgeOfTown() {}

// Start/Continue a new Game
auto Sorcery::EdgeOfTown::start() -> std::optional<MenuItem> {

	// Get the Background Display Components and load them into Display module storage (not local -
	// and note that due to the way both menus are combined in this class, we need to have the
	// menustage set first in this case and this case only)
	_display->generate_components("edge_of_town");

	// Clear the window
	_window->clear();

	// Draw the Custom Components
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
							option_chosen == MenuItem::ET_CASTLE) {
							return MenuItem::ET_CASTLE;
						} else if (option_chosen == MenuItem::ET_LEAVE_GAME) {
							_display->set_input_mode(WindowInputMode::CONFIRM_LEAVE_GAME);
							_yes_or_no = WindowConfirm::NO;
						} else if (option_chosen == MenuItem::ET_MAZE) {
							return MenuItem::ET_MAZE;
						} else if (option_chosen == MenuItem::ET_TRAIN) {
							_training->start();
							_training->stop();
							_display->generate_components("edge_of_town");
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
						}
					}
				} else if ((_system->input->check_for_event(WindowInput::CANCEL, event)) ||
						   ((_system->input->check_for_event(WindowInput::BACK, event)))) {
					_display->set_input_mode(WindowInputMode::CONFIRM_LEAVE_GAME);
					_yes_or_no = WindowConfirm::NO;
				}

			} else if (_display->get_input_mode() == WindowInputMode::CONFIRM_LEAVE_GAME) {

				auto dialog_input{_dialog_leave_game->handle_input(event)};
				if (dialog_input) {
					if (dialog_input.value() == WindowDialogButton::CLOSE) {
						return std::nullopt;
						_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
					} else if (dialog_input.value() == WindowDialogButton::YES) {
						_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
						return MenuItem::ET_LEAVE_GAME;
					} else if (dialog_input.value() == WindowDialogButton::NO) {
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

auto Sorcery::EdgeOfTown::stop() -> void {}

auto Sorcery::EdgeOfTown::_draw() -> void {

	// Custom Components
	_display->display_components("edge_of_town");
	_window->draw(*_status_bar);

	// And the Menu
	const double lerp{_graphics->animation->colour_lerp};

	_menu->generate((*_display->layout)["edge_of_town:menu"], lerp);
	const sf::Vector2f menu_pos(
		(*_display->layout)["edge_of_town:menu"].x, (*_display->layout)["edge_of_town:menu"].y);
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
