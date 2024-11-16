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

#include "modules/sell.hpp"
#include "common/enum.hpp"
#include "common/macro.hpp"
#include "common/type.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/input.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "engine/inspect.hpp"
#include "gui/console.hpp"
#include "gui/frame.hpp"
#include "gui/menu.hpp"
#include "gui/partypanel.hpp"
#include "resources/componentstore.hpp"
#include "resources/factory.hpp"
#include "types/character.hpp"
#include "types/component.hpp"

// Standard Constructor
Sorcery::Sell::Sell(System *system, Display *display, Graphics *graphics,
	Game *game, MIA action)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game},
	  _action(action) {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	/* // Setup Custom Components
	_menu =
		std::make_unique<Menu>(_system, _display, _graphics, _game, MTP::SHOP);
	_menu->generate((*_display->layout)["shop:menu"]);
	_menu->setPosition(_display->get_centre_x(_menu->get_width()),
		(*_display->layout)["shop:menu"].y);

	_who = std::make_unique<Menu>(
		_system, _display, _graphics, _game, MTP::PARTY_CHARACTERS, MMD::SHOP);
	_who->generate((*_display->layout)["shop_who:menu"]);
	_who->setPosition(_display->get_centre_x(_who->get_width()),
		(*_display->layout)["shop_who:menu"].y);

	_action = std::make_unique<Menu>(
		_system, _display, _graphics, _game, MTP::SHOP_ACTION);
	_action->generate((*_display->layout)["shop_action:menu"]);
	_action->setPosition(_display->get_centre_x(_action->get_width()),
		(*_display->layout)["shop_action:menu"].y); */

	// Modules
	_party_panel = std::make_unique<PartyPanel>(_system, _display, _graphics,
		_game, (*_display->layout)["global:party_panel"]);

	_console = std::make_unique<Console>(
		_display->window->get_gui(), _system, _display, _graphics, _game);
	_game->hide_console();
}

// Standard Destructor
Sorcery::Sell::~Sell() {
}

// Selling and other Shop Interactions
auto Sorcery::Sell::start(Character &character) -> std::optional<MIM> {

	_display->generate("sell");

	// Clear the window
	_window->clear();

	// Refresh the Party characters
	_party_panel->refresh();

	// TODO: generate the menu here

	// Generate the Components
	const Component party_banel_c{(*_display->layout)["global:party_panel"]};
	_party_panel->setPosition(_display->get_centre_x(_party_panel->width),
		(*_display->layout)["global:party_panel"].y);

	// And do the main loop
	_display->set_input_mode(WIM::NAVIGATE_MENU);

	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

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

				if (_system->input->check(CIN::SHOW_HIDE_CONSOLE, event))
					_game->toggle_console();

				if (_system->input->check(CIN::CANCEL, event))
					return std::nullopt;

				if (_system->input->check(CIN::BACK, event))
					return std::nullopt;
			}
		}

		_window->clear();

		_draw();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::Sell::stop() -> void {
}

auto Sorcery::Sell::_draw() -> void {

	// Play the background movie!
	_graphics->tile_bg(_window);

	// Custom Components
	_display->display("shop");
	_window->draw(*_party_panel);

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}
