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

#include "castle.hpp"

// Standard Constructor
Sorcery::Castle::Castle(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::CASTLE);
	_menu->generate((*_display->layout)["castle:menu"]);
	_menu->setPosition(_display->get_centre_x(_menu->get_width()), (*_display->layout)["castle:menu"].y);

	_leave_game =
		std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["castle:dialog_leave_game"],
			(*_display->layout)["castle:dialog_leave_game_text"], WindowDialogType::CONFIRM);
	_leave_game->setPosition(_display->get_centre_pos(_leave_game->get_size()));

	// Modules
	_party_panel =
		std::make_unique<PartyPanel>(_system, _display, _graphics, _game, (*_display->layout)["global:party_panel"]);
	_edge_of_town = std::make_unique<EdgeOfTown>(_system, _display, _graphics, _game);
	_tavern = std::make_unique<Tavern>(_system, _display, _graphics, _game);
	_inn = std::make_unique<Inn>(_system, _display, _graphics, _game);
	_shop = std::make_unique<Shop>(_system, _display, _graphics, _game);
	_temple = std::make_unique<Temple>(_system, _display, _graphics, _game);
}

// Standard Destructor
Sorcery::Castle::~Castle() {
}

// Start/Continue a new Game
auto Sorcery::Castle::start(Destination destination) -> std::optional<MenuItem> {

	// TODO: need to incorporare this in main loop so that exiting this goes back properly
	if (destination == Destination::MAZE) {
		if (auto edge_option{_edge_of_town->start(destination)};
			edge_option && edge_option.value() == MenuItem::ITEM_ABORT) {
			_game->save_game();
			_display->shutdown_SFML();
			return MenuItem::ITEM_ABORT;
		}
		_edge_of_town->stop();
	} else if (destination == Destination::TAVERN) {
		if (auto tavern_option{_tavern->start()}; tavern_option && tavern_option.value() == MenuItem::ITEM_ABORT) {
			_game->save_game();
			_display->shutdown_SFML();
			return MenuItem::ITEM_ABORT;
		}
		_tavern->stop();
	} else if (destination == Destination::TRAINING) {
		if (auto edge_option{_edge_of_town->start(destination)};
			edge_option && edge_option.value() == MenuItem::ITEM_ABORT) {
			_game->save_game();
			_display->shutdown_SFML();
			return MenuItem::ITEM_ABORT;
		}
		_edge_of_town->stop();
	} else if (destination == Destination::INN) {
		if (auto inn_option{_inn->start()}; inn_option && inn_option.value() == MenuItem::ITEM_ABORT) {
			_game->save_game();
			_display->shutdown_SFML();
			return MenuItem::ITEM_ABORT;
		}
		_inn->stop();
	} else if (destination == Destination::TEMPLE) {
		if (auto temple_option{_temple->start()}; temple_option && temple_option.value() == MenuItem::ITEM_ABORT) {
			_game->save_game();
			_display->shutdown_SFML();
			_temple->stop();
			return MenuItem::ITEM_ABORT;
		}
		_temple->stop();
	} else if (destination == Destination::SHOP) {
		if (auto shop_option{_shop->start()}; shop_option && shop_option.value() == MenuItem::ITEM_ABORT) {
			_game->save_game();
			_display->shutdown_SFML();
			_temple->stop();
			return MenuItem::ITEM_ABORT;
		}
		_shop->stop();
	}

	// Get the Background Display Components and load them into Display module storage (not local - and note that due to
	// the way both menus are combined in this class, we need to have the menu stage set first in this case and this
	// case only)
	_update_menus();
	_display->generate("castle");

	// Clear the window
	_window->clear();

	// Generate the Components
	const Component party_banel_c{(*_display->layout)["global:party_panel"]};
	_party_panel->setPosition(_display->get_centre_x(_party_panel->width), (*_display->layout)["global:party_panel"].y);

	// Refresh the Party characters
	_party_panel->refresh();

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
					return MenuItem::ITEM_ABORT;

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
					option = _menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
				else if (_system->input->check(WindowInput::CONFIRM, event)) {

					// We have selected something from the menu
					if (option) {
						const MenuItem option_chosen{(*option.value()).item};
						if (option_chosen == MenuItem::CA_EDGE_OF_TOWN) {
							auto edge_option{_edge_of_town->start()};
							_edge_of_town->stop();
							if (edge_option) {
								if (edge_option.value() == MenuItem::ET_LEAVE_GAME)
									return MenuItem::ET_LEAVE_GAME;
								else if (edge_option.value() == MenuItem::ITEM_ABORT) {
									_game->save_game();
									_display->shutdown_SFML();
									return MenuItem::ITEM_ABORT;
								}
							}
							_display->generate("castle");
							_party_panel->refresh();
							_update_menus();
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							continue;
						} else if (option_chosen == MenuItem::CA_TAVERN) {
							if (auto tavern_option{_tavern->start()};
								tavern_option && tavern_option.value() == MenuItem::ITEM_ABORT) {
								_game->save_game();
								_display->shutdown_SFML();
								return MenuItem::ITEM_ABORT;
							}
							_tavern->stop();
							_game->save_game();
							_party_panel->refresh();
							_update_menus();
							_display->generate("castle");
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							continue;
						} else if (option_chosen == MenuItem::CA_INN) {
							if (auto inn_option{_inn->start()};
								inn_option && inn_option.value() == MenuItem::ITEM_ABORT) {
								_game->save_game();
								_display->shutdown_SFML();
								return MenuItem::ITEM_ABORT;
							}
							_inn->stop();
							_game->save_game();
							_party_panel->refresh();
							_display->generate("castle");
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							continue;
						} else if (option_chosen == MenuItem::CA_SHOP) {
							if (auto shop_option{_shop->start()};
								shop_option && shop_option.value() == MenuItem::ITEM_ABORT) {
								_game->save_game();
								_display->shutdown_SFML();
								return MenuItem::ITEM_ABORT;
							}
							_shop->stop();
							_game->save_game();
							_party_panel->refresh();
							_display->generate("castle");
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							continue;
						} else if (option_chosen == MenuItem::CA_TEMPLE) {
							if (auto temple_option{_temple->start()};
								temple_option && temple_option.value() == MenuItem::ITEM_ABORT) {
								_game->save_game();
								_display->shutdown_SFML();
								return MenuItem::ITEM_ABORT;
							}
							_temple->stop();
							_game->save_game();
							_party_panel->refresh();
							_display->generate("castle");
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							continue;
						}
					}
				} else if ((_system->input->check(WindowInput::CANCEL, event)) ||
						   ((_system->input->check(WindowInput::BACK, event)))) {
					_display->set_input_mode(WindowInputMode::CONFIRM_LEAVE_GAME);
				}
			} else if (_display->get_input_mode() == WindowInputMode::CONFIRM_LEAVE_GAME) {

				auto dialog_input{_leave_game->handle_input(event)};
				if (dialog_input) {
					if (dialog_input.value() == WindowDialogButton::CLOSE) {
						_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
						return std::nullopt;
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

auto Sorcery::Castle::stop() -> void {
}

auto Sorcery::Castle::_update_menus() -> void {

	Component component{(*_display->layout)["castle:menu"]};
	if (_game->state->get_party_characters().size() == 0) {
		_menu->disable_entry(component, 1);
		_menu->disable_entry(component, 2);
		_menu->disable_entry(component, 3);
	} else {
		_menu->enable_entry(component, 1);
		_menu->enable_entry(component, 2);
		_menu->enable_entry(component, 3);
	}
}

auto Sorcery::Castle::_draw() -> void {

	// Background Wallpaper
	_graphics->tile_bg(_window);

	// Custom Components
	_display->display("castle");
	_window->draw(*_party_panel);

	// And the Menu
	_menu->generate((*_display->layout)["castle:menu"]);
	_window->draw(*_menu);
	if (_display->get_input_mode() == WindowInputMode::CONFIRM_LEAVE_GAME) {
		_leave_game->update();
		_window->draw(*_leave_game);
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}
