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

#include "modules/buy.hpp"
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
#include "gui/menupaged.hpp"
#include "gui/partypanel.hpp"
#include "resources/componentstore.hpp"
#include "resources/factory.hpp"
#include "types/character.hpp"
#include "types/component.hpp"

// Standard Constructor
Sorcery::Buy::Buy(
	System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Modules
	_party_panel = std::make_unique<PartyPanel>(_system, _display, _graphics,
		_game, (*_display->layout)["global:party_panel"]);

	_console = std::make_unique<Console>(
		_display->window->get_gui(), _system, _display, _graphics, _game);
	_game->hide_console();

	_action_gold = sf::Text();
	_action_name = sf::Text();
}

// Standard Destructor
Sorcery::Buy::~Buy() {
}

// Buying from the Shop
auto Sorcery::Buy::start(const unsigned int character_id)
	-> std::optional<MIM> {

	_display->generate("buy");

	// Clear the window
	_window->clear();

	// Refresh the Party characters
	_party_panel->refresh();

	// Setup Custom Components

	_menu = std::make_unique<MenuPaged>(
		_system, _display, _graphics, _game, MTP::BUY_ITEMS, 7);
	_menu->set_current_page(0);

	const auto menu_c{(*_display->layout)["buy:menu"]};
	_menu->generate(menu_c, true);
	_menu->setPosition(_display->get_centre_x(_menu->get_width()),
		(*_display->layout)["sell:menu"].y);

	// Generate the Components
	const Component party_panel_c{(*_display->layout)["global:party_panel"]};
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

auto Sorcery::Buy::stop() -> void {
}

auto Sorcery::Buy::_draw() -> void {

	// Play the background movie!
	_graphics->tile_bg(_window);

	// Custom Components
	_display->display("buy");
	_window->draw(*_party_panel);

	_menu->generate((*_display->layout)["shop_action:menu"]);
	_window->draw(*_menu);

	_display->window->draw_text(
		_action_gold, (*_display->layout)["shop_action:action_gold"], _gold);
	_display->window->draw_text(
		_action_name, (*_display->layout)["shop_action:action_name"], _name);

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}

auto Sorcery::Buy::_update_welcome() -> void {

	const auto character{_game->characters[_chosen_char_id]};

	_gold = fmt::format("{} {} {}", (*_display->string)["SHOP_GOLD_PREFIX"],
		character.get_gold(), (*_display->string)["SHOP_GOLD_SUFFIX"]);
	_name = fmt::format("{} {}", (*_display->string)["SHOP_WELCOME_PREFIX"],
		character.get_name());
}