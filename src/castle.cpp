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
Sorcery::Castle::Castle(
	System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_menu = std::make_shared<Menu>(
		_system, _display, _graphics, _game, MenuType::CASTLE);

	_dlg_leave = std::make_shared<Dialog>(_system, _display, _graphics,
		(*_display->layout)["castle:dialog_leave_game"],
		(*_display->layout)["castle:dialog_leave_game_text"],
		WindowDialogType::CONFIRM);
	_dlg_leave->setPosition((*_display->layout)["castle:dialog_leave_game"].x,
		(*_display->layout)["castle:dialog_leave_game"].y);

	// Modules
	_status_bar = std::make_unique<StatusBar>(_system, _display, _graphics);
	_edge_of_town =
		std::make_shared<EdgeOfTown>(_system, _display, _graphics, _game);
}

// Standard Destructor
Sorcery::Castle::~Castle() {}

// Start/Continue a new Game
auto Sorcery::Castle::start() -> std::optional<MenuItem> {

	// Get the Background Display Components and load them into Display module
	// storage (not local - and note that due to the way both menus are combined
	// in this class, we need to have the menu stage set first in this case and
	// this case only)
	_display->generate("castle");

	// Clear the window
	_window->clear();

	// Generate the Components
	const Component status_bar_c{(*_display->layout)["status_bar:status_bar"]};
	_status_bar->setPosition(
		_display->window->get_x(_status_bar->sprite, status_bar_c.x),
		_display->window->get_y(_status_bar->sprite, status_bar_c.y));

	// Play the background movie!
	_display->fit_bg_movie();
	_display->start_bg_movie();

	// And do the main loop
	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
	std::optional<std::vector<MenuEntry>::const_iterator> option{
		_menu->items.begin()};
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// If we are in normal input mode
			if (_display->get_input_mode() == WindowInputMode::NAVIGATE_MENU) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					_window->close();

				// Handle enabling help overlay
				if (_system->input->check_for_event(
						WindowInput::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				// And handle input on the main menu
				if (_system->input->check_for_event(WindowInput::UP, event))
					option = _menu->choose_previous();
				else if (_system->input->check_for_event(
							 WindowInput::DOWN, event))
					option = _menu->choose_next();
				else if (_system->input->check_for_event(
							 WindowInput::MOVE, event))
					option =
						_menu->set_mouse_selected(static_cast<sf::Vector2f>(
							sf::Mouse::getPosition(*_window)));
				else if (_system->input->check_for_event(
							 WindowInput::CONFIRM, event)) {

					// We have selected something from the menu
					if (option) {
						if (const MenuItem option_chosen{
								(*option.value()).item};
							option_chosen == MenuItem::CA_EDGE_OF_TOWN) {
							auto edge_option{_edge_of_town->start()};
							_edge_of_town->stop();
							if (edge_option) {
								if (edge_option.value() ==
									MenuItem::ET_LEAVE_GAME)
									return MenuItem::ET_LEAVE_GAME;
							}
							_display->generate("castle");
							_display->set_input_mode(
								WindowInputMode::NAVIGATE_MENU);
							continue;
						}
					}
				} else if ((_system->input->check_for_event(
							   WindowInput::CANCEL, event)) ||
						   ((_system->input->check_for_event(
							   WindowInput::BACK, event)))) {
					_display->set_input_mode(
						WindowInputMode::CONFIRM_LEAVE_GAME);
					_yes_or_no = WindowConfirm::NO;
				}
			} else if (_display->get_input_mode() ==
					   WindowInputMode::CONFIRM_LEAVE_GAME) {

				auto dialog_input{_dlg_leave->handle_input(event)};
				if (dialog_input) {
					if (dialog_input.value() == WindowDialogButton::CLOSE) {
						return std::nullopt;
						_display->set_input_mode(
							WindowInputMode::NAVIGATE_MENU);
					} else if (dialog_input.value() ==
							   WindowDialogButton::YES) {
						_display->set_input_mode(
							WindowInputMode::NAVIGATE_MENU);
						return MenuItem::ET_LEAVE_GAME;
					} else if (dialog_input.value() == WindowDialogButton::NO) {
						_display->set_input_mode(
							WindowInputMode::NAVIGATE_MENU);
					}
				}
			}
		}

		_window->clear();

		// Update Background Movie
		_display->start_bg_movie();
		_display->update_background_movie();
		_display->draw_background_movie();

		_draw();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::Castle::stop() -> void {

	// Stop the background movie!
	_display->stop_background_movie();
}

auto Sorcery::Castle::_draw() -> void {

	// Custom Components
	_display->display("castle");
	_window->draw(*_status_bar);

	// And the Menu
	_menu->generate((*_display->layout)["castle:menu"]);
	const sf::Vector2f menu_pos((*_display->layout)["castle:menu"].x,
		(*_display->layout)["castle:menu"].y);
	_menu->setPosition(menu_pos);
	_window->draw(*_menu);
	if (_display->get_input_mode() == WindowInputMode::CONFIRM_LEAVE_GAME) {
		_dlg_leave->update();
		_window->draw(*_dlg_leave);
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}
