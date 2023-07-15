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

#include "shop.hpp"

// Standard Constructor
Sorcery::Shop::Shop(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::SHOP);
	_menu->generate((*_display->layout)["shop:menu"]);
	_menu->setPosition(_display->get_centre_x(_menu->get_width()), (*_display->layout)["shop:menu"].y);

	// Modules
	_party_panel =
		std::make_unique<PartyPanel>(_system, _display, _graphics, _game, (*_display->layout)["global:party_panel"]);
	_inspect = std::make_unique<Inspect>(_system, _display, _graphics, _game, MenuMode::SHOP);
}

// Standard Destructor
Sorcery::Shop::~Shop() {
}

// Visit the Tavern
auto Sorcery::Shop::start() -> std::optional<MenuItem> {

	// Get the Background Display Components and load them into Display module storage (not local - and note that due to
	// the way both menus are combined in this class, we need to have the menu stage set first in this case and this
	// case only)
	_display->generate("shop");

	// Clear the window
	_window->clear();

	// Refresh the Party characters
	_party_panel->refresh();

	// Generate the Components
	const Component party_banel_c{(*_display->layout)["global:party_panel"]};
	_party_panel->setPosition(_display->get_centre_x(_party_panel->width), (*_display->layout)["global:party_panel"].y);

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

				if (_system->input->check(WindowInput::CANCEL, event))
					return std::nullopt;

				if (_system->input->check(WindowInput::BACK, event))
					return std::nullopt;

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
						if (const MenuItem option_chosen{(*option.value()).item};
							option_chosen == MenuItem::SH_CASTLE) {
							return MenuItem::SH_CASTLE;
						} else if (option_chosen == MenuItem::SH_INSPECT) {

							auto result{_inspect->start()};
							if (result && result.value() == MenuItem::ITEM_ABORT) {
								_inspect->stop();
								return MenuItem::ITEM_ABORT;
							}

							_inspect->stop();
							_display->generate("shop");
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							continue;
						}
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

auto Sorcery::Shop::stop() -> void {
}

auto Sorcery::Shop::_draw() -> void {

	// Play the background movie!
	_graphics->tile_bg(_window);

	// Custom Components
	_display->display("shop");
	_window->draw(*_party_panel);

	// And the Menu
	_menu->generate((*_display->layout)["shop:menu"]);
	_window->draw(*_menu);

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}
