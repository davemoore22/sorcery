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

#include "modules/castle.hpp"
#include "common/type.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/input.hpp"
#include "core/state.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "gui/dialog.hpp"
#include "gui/frame.hpp"
#include "gui/menu.hpp"
#include "gui/partypanel.hpp"
#include "resources/componentstore.hpp"
#include "resources/factory.hpp"
#include "types/component.hpp"

// Standard Constructor
Sorcery::Castle::Castle(
	System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_menu = std::make_unique<Menu>(
		_system, _display, _graphics, _game, MTP::CASTLE);
	_menu->generate((*_display->layout)["castle:menu"]);
	_menu->setPosition(_display->get_centre_x(_menu->get_width()),
		(*_display->layout)["castle:menu"].y);

	_leave_game = std::make_unique<Dialog>(_system, _display, _graphics,
		(*_display->layout)["castle:dialog_leave_game"],
		(*_display->layout)["castle:dialog_leave_game_text"], WDT::CONFIRM);
	_leave_game->setPosition(_display->get_centre_pos(_leave_game->get_size()));

	// Modules
	_party_panel = std::make_unique<PartyPanel>(_system, _display, _graphics,
		_game, (*_display->layout)["global:party_panel"]);
}

// Standard Destructor
Sorcery::Castle::~Castle() {
}

// Start/Continue a new Game
auto Sorcery::Castle::start(DES destination) -> std::optional<MIM> {

	if (destination == DES::RESTART)
		return MIM::ET_RESTART;
	else if (destination == DES::EDGE)
		return MIM::CA_EDGE_OF_TOWN;
	else if (destination == DES::MAZE)
		return MIM::ET_MAZE;

	// Get the Background Display Components and load them into Display module
	// storage (not local - and note that due to the way both menus are combined
	// in this class, we need to have the menu stage set first in this case and
	// this case only)
	_update_menus();
	_display->generate("castle");

	// Clear the window
	_window->clear();

	// Generate the Components
	const Component party_banel_c{(*_display->layout)["global:party_panel"]};
	_party_panel->setPosition(_display->get_centre_x(_party_panel->width),
		(*_display->layout)["global:party_panel"].y);

	// Refresh the Party characters
	_party_panel->refresh();

	// And do the main loop
	_display->set_input_mode(WIM::NAVIGATE_MENU);
	MenuSelect option{_menu->items.begin()};
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			if ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::F10)) {
				_game->debug_create_random_party();
				_party_panel->refresh();
				_update_menus();
			} else if ((event.type == sf::Event::KeyPressed) &&
					   (event.key.code == sf::Keyboard::F9)) {
				_game->debug_fill_party_unid_items();
				_party_panel->refresh();
				_update_menus();
			}

			// If we are in normal input mode
			if (_display->get_input_mode() == WIM::NAVIGATE_MENU) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return MIM::ITEM_ABORT;

				// Handle enabling help overlay
				if (_system->input->check(CIN::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				// And handle input on the main menu
				if (_system->input->check(CIN::UP, event))
					option = _menu->choose_previous();
				else if (_system->input->check(CIN::DOWN, event))
					option = _menu->choose_next();
				else if (_system->input->check(CIN::MOVE, event))
					option = _menu->set_mouse_selected(_display->get_cur());
				else if (_system->input->check(CIN::CONFIRM, event)) {

					// We have selected something from the menu
					if (option) {
						const MIM opt{(*option.value()).item};
						if (opt == MIM::CA_EDGE_OF_TOWN)
							return MIM::CA_EDGE_OF_TOWN;
						else if (opt == MIM::CA_TAVERN)
							return MIM::CA_TAVERN;
						else if (opt == MIM::CA_INN)
							return MIM::CA_INN;
						else if (opt == MIM::CA_SHOP)
							return MIM::CA_SHOP;
						else if (opt == MIM::CA_TEMPLE)
							return MIM::CA_TEMPLE;
					}
				} else if (_system->input->check(CIN::CANCEL, event) ||
						   _system->input->check(CIN::BACK, event)) {
					_display->set_input_mode(WIM::CONFIRM_LEAVE_GAME);
				}
			} else if (_display->get_input_mode() == WIM::CONFIRM_LEAVE_GAME) {

				auto dialog_input{_leave_game->handle_input(event)};
				if (dialog_input) {
					if (dialog_input.value() == WDB::CLOSE) {
						_display->set_input_mode(WIM::NAVIGATE_MENU);
						_game->save_game();
						return MIM::ITEM_LEAVE_GAME;
					} else if (dialog_input.value() == WDB::YES) {
						_display->set_input_mode(WIM::NAVIGATE_MENU);
						return MIM::ITEM_LEAVE_GAME;
					} else if (dialog_input.value() == WDB::NO) {
						_display->set_input_mode(WIM::NAVIGATE_MENU);
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
	if (_display->get_input_mode() == WIM::CONFIRM_LEAVE_GAME) {
		_leave_game->update();
		_window->draw(*_leave_game);
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}
