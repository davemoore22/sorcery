// Copyright (C) 2021 Dave Moore
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

#include "castle.hpp"

// Standard Constructor
Sorcery::Castle::Castle(
	System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_menu = std::make_unique<Menu>(
		_system, _display, _graphics, _game, MenuType::CASTLE);

	_leave_game = std::make_unique<Dialog>(_system, _display, _graphics,
		(*_display->layout)["castle:dialog_leave_game"],
		(*_display->layout)["castle:dialog_leave_game_text"],
		WindowDialogType::CONFIRM);
	_leave_game->setPosition((*_display->layout)["castle:dialog_leave_game"].x,
		(*_display->layout)["castle:dialog_leave_game"].y);

	// Modules
	_status_bar =
		std::make_unique<StatusBar>(_system, _display, _graphics, _game);
	_edge_of_town =
		std::make_unique<EdgeOfTown>(_system, _display, _graphics, _game);
	_tavern = std::make_unique<Tavern>(_system, _display, _graphics, _game);
	_inn = std::make_unique<Inn>(_system, _display, _graphics, _game);
	_shop = std::make_unique<Shop>(_system, _display, _graphics, _game);
	_temple = std::make_unique<Temple>(_system, _display, _graphics, _game);
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

	// Refresh the Party characters
	_status_bar->refresh();
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
				if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				// And handle input on the main menu
				if (_system->input->check(WindowInput::UP, event))
					option = _menu->choose_previous();
				else if (_system->input->check(WindowInput::DOWN, event))
					option = _menu->choose_next();
				else if (_system->input->check(WindowInput::MOVE, event))
					option =
						_menu->set_mouse_selected(static_cast<sf::Vector2f>(
							sf::Mouse::getPosition(*_window)));
				else if (_system->input->check(WindowInput::CONFIRM, event)) {

					// We have selected something from the menu
					if (option) {
						const MenuItem option_chosen{(*option.value()).item};
						if (option_chosen == MenuItem::CA_EDGE_OF_TOWN) {
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
						} else if (option_chosen == MenuItem::CA_TAVERN) {
							auto tavern_option{_tavern->start()};
							_tavern->stop();
							_game->save_game();
							_status_bar->refresh();
							_display->generate("castle");
							_display->set_input_mode(
								WindowInputMode::NAVIGATE_MENU);
							continue;
						} else if (option_chosen == MenuItem::CA_INN) {
							auto inn_option{_inn->start()};
							_inn->stop();
							_game->save_game();
							_status_bar->refresh();
							_display->generate("castle");
							_display->set_input_mode(
								WindowInputMode::NAVIGATE_MENU);
							continue;
						} else if (option_chosen == MenuItem::CA_SHOP) {
							auto shop_option{_shop->start()};
							_shop->stop();
							_game->save_game();
							_status_bar->refresh();
							_display->generate("castle");
							_display->set_input_mode(
								WindowInputMode::NAVIGATE_MENU);
							continue;
						} else if (option_chosen == MenuItem::CA_TEMPLE) {
							auto temple_option{_temple->start()};
							_temple->stop();
							_game->save_game();
							_status_bar->refresh();
							_display->generate("castle");
							_display->set_input_mode(
								WindowInputMode::NAVIGATE_MENU);
							continue;
						}
					}
				} else if ((_system->input->check(
							   WindowInput::CANCEL, event)) ||
						   ((_system->input->check(
							   WindowInput::BACK, event)))) {
					_display->set_input_mode(
						WindowInputMode::CONFIRM_LEAVE_GAME);
				}
			} else if (_display->get_input_mode() ==
					   WindowInputMode::CONFIRM_LEAVE_GAME) {

				auto dialog_input{_leave_game->handle_input(event)};
				if (dialog_input) {
					if (dialog_input.value() == WindowDialogButton::CLOSE) {
						_display->set_input_mode(
							WindowInputMode::NAVIGATE_MENU);
						return std::nullopt;
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
		_display->update_bg_movie();
		_display->draw_bg_movie();

		_draw();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::Castle::stop() -> void {

	// Stop the background movie!
	_display->stop_bg_movie();
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
		_leave_game->update();
		_window->draw(*_leave_game);
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}
