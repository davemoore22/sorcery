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

#include "modules/edgeoftown.hpp"
#include "common/enum.hpp"
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
#include "types/character.hpp"
#include "types/component.hpp"

// Standard Constructor
Sorcery::EdgeOfTown::EdgeOfTown(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MTP::EDGE_OF_TOWN);
	_menu->generate((*_display->layout)["edge_of_town:menu"]);
	_menu->setPosition(_display->get_centre_x(_menu->get_width()), (*_display->layout)["edge_of_town:menu"].y);

	_leave_game =
		std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["edge_of_town:dialog_leave_game"],
			(*_display->layout)["edge_of_town:dialog_leave_game_text"], WDT::CONFIRM);
	_leave_game->setPosition(_display->get_centre_pos(_leave_game->get_size()));

	// Modules
	_party_panel =
		std::make_unique<PartyPanel>(_system, _display, _graphics, _game, (*_display->layout)["global:party_panel"]);
}

// Standard Destructor
Sorcery::EdgeOfTown::~EdgeOfTown() {
}

// Start/Continue a new Game
auto Sorcery::EdgeOfTown::start(DES destination) -> std::optional<MIM> {

	if (destination == DES::RESTART)
		return MIM::ET_RESTART;
	else if (destination == DES::MAZE)
		return MIM::ET_MAZE;

	_update_menus();
	_display->generate("edge_of_town");

	// Clear the window
	_window->clear();

	// Refresh the Party characters
	_party_panel->refresh();
	_party_panel->setPosition(_display->get_centre_x(_party_panel->width), (*_display->layout)["global:party_panel"].y);

	// And do the main loop
	_display->set_input_mode(WIM::NAVIGATE_MENU);
	std::optional<std::vector<MenuEntry>::const_iterator> opt{_menu->items.begin()};
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// If we are in normal input mode
			if (_display->get_input_mode() == WIM::NAVIGATE_MENU) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					_window->close();

				// Handle enabling help overlay
				if (_system->input->check(CIN::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				// And handle input on the main menu
				if (_system->input->check(CIN::UP, event))
					opt = _menu->choose_previous();
				else if (_system->input->check(CIN::DOWN, event))
					opt = _menu->choose_next();
				else if (_system->input->check(CIN::MOVE, event))
					opt = _menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
				else if (_system->input->check(CIN::CONFIRM, event)) {

					// We have selected something from the menu
					if (opt) {
						if (const MIM opt_edge{(*opt.value()).item}; opt_edge == MIM::ET_CASTLE) {
							return MIM::ET_CASTLE;
						} else if (opt_edge == MIM::ET_LEAVE_GAME)
							_display->set_input_mode(WIM::CONFIRM_LEAVE_GAME);
						else if (opt_edge == MIM::ET_MAZE)
							return MIM::ET_MAZE;
						else if (opt_edge == MIM::ET_TRAIN)
							return MIM::ET_TRAIN;
						else if (opt_edge == MIM::ET_RESTART)
							return MIM::ET_RESTART;
					}
				} else if (_system->input->check(CIN::CANCEL, event) || _system->input->check(CIN::BACK, event)) {
					_display->set_input_mode(WIM::CONFIRM_LEAVE_GAME);
				}

			} else if (_display->get_input_mode() == WIM::CONFIRM_LEAVE_GAME) {

				auto dialog_input{_leave_game->handle_input(event)};
				if (dialog_input) {
					if (dialog_input.value() == WDB::CLOSE) {
						_display->set_input_mode(WIM::NAVIGATE_MENU);
						_game->save_game();
						return MIM::ET_LEAVE_GAME;
					} else if (dialog_input.value() == WDB::YES) {
						_display->set_input_mode(WIM::NAVIGATE_MENU);

						// If we leave the game, move any current party members into the Tavern
						for (auto &[character_id, character] : _game->characters) {
							if (character.get_location() == CHL::PARTY)
								character.set_location(CHL::TAVERN);
						}
						_game->state->clear_party();
						_game->save_game();
						return MIM::ET_LEAVE_GAME;
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

auto Sorcery::EdgeOfTown::stop() -> void {
}

auto Sorcery::EdgeOfTown::_update_menus() -> void {

	Component component{(*_display->layout)["edge_of_town:menu"]};
	if (_game->state->get_party_characters().size() == 0) {
		_menu->disable_entry(component, 2);
	} else {
		_menu->enable_entry(component, 2);
	}

	if (_game->has_party_in_maze()) {
		_menu->enable_entry(component, 3);
	} else {
		_menu->disable_entry(component, 3);
	}
}

auto Sorcery::EdgeOfTown::_draw() -> void {

	// Background Wallpaper
	_graphics->tile_bg(_window);

	// Custom Components
	_display->display("edge_of_town");
	_window->draw(*_party_panel);

	// And the Menu
	_menu->generate((*_display->layout)["edge_of_town:menu"]);
	_window->draw(*_menu);
	if (_display->get_input_mode() == WIM::CONFIRM_LEAVE_GAME) {
		_leave_game->update();
		_window->draw(*_leave_game);
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}
