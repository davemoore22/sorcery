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

#include "modules/shop.hpp"
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
#include "gui/dialog.hpp"
#include "gui/frame.hpp"
#include "gui/menu.hpp"
#include "gui/partypanel.hpp"
#include "modules/buy.hpp"
#include "modules/sell.hpp"
#include "resources/componentstore.hpp"
#include "resources/factory.hpp"
#include "types/component.hpp"

// Standard Constructor
Sorcery::Shop::Shop(
	System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup the Factory
	_factory = std::make_unique<Factory>(_system, _display, _graphics, _game);

	// Setup Custom Components
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
		(*_display->layout)["shop_action:menu"].y);

	// Modules
	_party_panel = std::make_unique<PartyPanel>(_system, _display, _graphics,
		_game, (*_display->layout)["global:party_panel"]);
	_inspect = std::make_unique<Inspect>(
		_system, _display, _graphics, _game, MMD::SHOP);
	_pool = _factory->make_dialog("engine_base_ui:dialog_pool_gold_ok");

	_console = std::make_unique<Console>(
		_display->window->get_gui(), _system, _display, _graphics, _game);
	_game->hide_console();

	_action_gold = sf::Text();
	_action_name = sf::Text();
}

// Standard Destructor
Sorcery::Shop::~Shop() {
}

// Visit the Shop
auto Sorcery::Shop::start() -> std::optional<MIM> {

	// Get the Background Display Components and load them into Display module
	// storage (not local - and note that due to the way both menus are combined
	// in this class, we need to have the menu stage set first in this case and
	// this case only)
	_display->generate("shop");
	_display->generate("shop_who", _w_sprites, _w_texts, _w_frames);
	_display->generate("shop_action", _a_sprites, _a_texts, _a_frames);

	// Clear the window
	_window->clear();

	// Refresh the Party characters
	_party_panel->refresh();
	_who->reload();
	_who->generate((*_display->layout)["shop_who:menu"]);

	// Generate the Components
	const Component party_banel_c{(*_display->layout)["global:party_panel"]};
	_party_panel->setPosition(_display->get_centre_x(_party_panel->width),
		(*_display->layout)["global:party_panel"].y);

	_stage = STS::MENU;

	_in_pool = false;

	// And do the main loop
	_display->set_input_mode(WIM::NAVIGATE_MENU);
	MenuSelect opt{_menu->items.begin()};
	MenuSelect opt_who{_who->items.begin()};
	MenuSelect opt_act{_action->items.begin()};
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			if (event.type == sf::Event::Closed)
				return MIM::ITEM_ABORT;
			else if (_system->input->check(CIN::CANCEL, event) ||
					 _system->input->check(CIN::BACK, event)) {
				_display->set_input_mode(WIM::NAVIGATE_MENU);
				_in_pool = false;
				return std::nullopt;
			}

			if (_in_pool) {

				// Handle Pool Gold Dialog
				auto input{_pool->handle_input(event)};
				if (input) {
					if (input.value() == WDB::CLOSE) {
						_in_pool = false;
						_display->set_input_mode(WIM::NAVIGATE_MENU);
					} else if (input.value() == WDB::OK) {

						_game->pool_party_gold(_chosen_char_id);
						_game->save_game();
						_in_pool = false;
						_display->set_input_mode(WIM::NAVIGATE_MENU);
					}
				}
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

				if (_system->input->check(CIN::SHOW_HIDE_CONSOLE, event))
					_game->toggle_console();

				if (_system->input->check(CIN::CANCEL, event))
					return std::nullopt;

				if (_system->input->check(CIN::BACK, event))
					return std::nullopt;

				// And handle input on the main menu
				if (_stage == STS::MENU) {
					if (_system->input->check(CIN::UP, event))
						opt = _menu->choose_previous();
					else if (_system->input->check(CIN::DOWN, event))
						opt = _menu->choose_next();
					else if (_system->input->check(CIN::MOVE, event))
						opt = _menu->set_mouse_selected(_display->get_cur());
					else if (_system->input->check(CIN::CONFIRM, event)) {

						// We have selected something from the menu
						if (opt) {
							if (const MIM opt_shop{(*opt.value()).item};
								opt_shop == MIM::SH_CASTLE) {
								return MIM::SH_CASTLE;
							} else if (opt_shop == MIM::SH_INSPECT) {

								auto result{_inspect->start(std::nullopt)};
								if (result &&
									result.value() == MIM::ITEM_ABORT) {
									_inspect->stop();
									return MIM::ITEM_ABORT;
								}

								_inspect->stop();
								_display->generate("shop");
								_display->set_input_mode(WIM::NAVIGATE_MENU);
								continue;
							} else if (opt_shop == MIM::SH_BUY_AND_SELL) {
								_stage = STS::WHO;
								_display->set_input_mode(WIM::NAVIGATE_MENU);
								continue;
							}
						}
					}
				} else if (_stage == STS::WHO) {
					if (_system->input->check(CIN::CANCEL, event)) {
						_stage = STS::MENU;
						_party_panel->refresh();
						_menu->generate((*_display->layout)["shop:menu"]);
					} else if (_system->input->check(CIN::BACK, event)) {
						_stage = STS::MENU;
						_party_panel->refresh();
						_menu->generate((*_display->layout)["shop:menu"]);
					} else if (_system->input->check(CIN::UP, event))
						opt_who = _who->choose_previous();
					else if (_system->input->check(CIN::DOWN, event))
						opt_who = _who->choose_next();
					else if (_system->input->check(CIN::MOVE, event))
						opt_who = _who->set_mouse_selected(_display->get_cur());
					else if (_system->input->check(CIN::CONFIRM, event)) {

						// We have selected something from the menu
						if (opt_who) {
							if (const MIM opt_shop{(*opt_who.value()).item};
								opt_shop == MIM::CA_SHOP) {
								_stage = STS::MENU;
								_party_panel->refresh();
								_menu->generate(
									(*_display->layout)["shop:menu"]);
								continue;
							} else {
								_chosen_char_id = (*opt_who.value()).index;
								_stage = STS::ACTION;
								_party_panel->refresh();
								_update_welcome();
								_action->generate(
									(*_display->layout)["shop_action:menu"]);
								continue;
							}
						}
					}
				} else if (_stage == STS::ACTION) {

					if (_system->input->check(CIN::CANCEL, event)) {
						_stage = STS::WHO;
						_party_panel->refresh();
						_who->generate((*_display->layout)["shop_who:menu"]);
					} else if (_system->input->check(CIN::BACK, event)) {
						_stage = STS::WHO;
						_party_panel->refresh();
						_who->generate((*_display->layout)["shop_who:menu"]);
					} else if (_system->input->check(CIN::UP, event))
						opt_act = _action->choose_previous();
					else if (_system->input->check(CIN::DOWN, event))
						opt_act = _action->choose_next();
					else if (_system->input->check(CIN::MOVE, event))
						opt_act =
							_action->set_mouse_selected(_display->get_cur());
					else if (_system->input->check(CIN::CONFIRM, event)) {
						if (const MIM opt_shop{(*opt_act.value()).item};
							opt_shop == MIM::SH_BACK) {
							_stage = STS::WHO;
							_party_panel->refresh();
							_who->generate(
								(*_display->layout)["shop_who:menu"]);
							continue;
						} else {

							if (opt_act) {
								if (const MIM opt_what{(*opt_act.value()).item};
									opt_what == MIM::SH_SELL) {

									_sell = std::make_unique<Sell>(_system,
										_display, _graphics, _game, MIA::SELL);
									auto result{_sell->start(_chosen_char_id)};
									if (result &&
										result.value() == MIM::ITEM_ABORT) {
										_sell->stop();
										continue;
									}
								} else if (opt_what == MIM::SH_BUY) {
									_buy = std::make_unique<Buy>(
										_system, _display, _graphics, _game);
									auto result{_buy->start(_chosen_char_id)};
									if (result &&
										result.value() == MIM::ITEM_ABORT) {
										_buy->stop();
										continue;
									}

								} else if (opt_what == MIM::SH_POOL_GOLD) {
									_in_pool = true;
									continue;
								} else if (opt_what == MIM::SH_IDENTIFY) {
									_sell = std::make_unique<Sell>(_system,
										_display, _graphics, _game,
										MIA::IDENTIFY);
									auto result{_sell->start(_chosen_char_id)};
									if (result &&
										result.value() == MIM::ITEM_ABORT) {
										_sell->stop();
										continue;
									}
								} else if (opt_what == MIM::SH_UNCURSE) {
									_sell = std::make_unique<Sell>(_system,
										_display, _graphics, _game,
										MIA::UNCURSE);
									auto result{_sell->start(_chosen_char_id)};
									if (result &&
										result.value() == MIM::ITEM_ABORT) {
										_sell->stop();
										continue;
									}
								} else if (opt_what == MIM::SH_BACK) {
									_stage = STS::WHO;
									_party_panel->refresh();
									_who->generate(
										(*_display->layout)["shop_who:menu"]);
									continue;
								}
							}
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
	if (_stage == STS::MENU) {
		_menu->generate((*_display->layout)["shop:menu"]);
		_window->draw(*_menu);
	} else if (_stage == STS::WHO) {

		_who->generate((*_display->layout)["shop_who:menu"]);
		_display->display("shop_who", _w_sprites, _w_texts, _w_frames);
		_window->draw(*_who);
	} else if (_stage == STS::ACTION) {

		_action->generate((*_display->layout)["shop_action:menu"]);
		_display->display("shop_action", _a_sprites, _a_texts, _a_frames);
		_window->draw(*_action);
		_display->window->draw_text(_action_gold,
			(*_display->layout)["shop_action:action_gold"], _gold);
		_display->window->draw_text(_action_name,
			(*_display->layout)["shop_action:action_name"], _name);

		if (_in_pool) {
			_pool->update();
			_window->draw(*_pool);
		}
	}

	if (_game->get_console_status()) {
		_console->refresh();
		_display->window->get_gui()->draw();
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}

auto Sorcery::Shop::_update_welcome() -> void {

	const auto character{_game->characters[_chosen_char_id]};

	_gold = fmt::format("{} {} {}", (*_display->string)["SHOP_GOLD_PREFIX"],
		character.get_gold(), (*_display->string)["SHOP_GOLD_SUFFIX"]);
	_name = fmt::format("{} {}", (*_display->string)["SHOP_WELCOME_PREFIX"],
		character.get_name());
}
