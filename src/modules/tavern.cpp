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

#include "modules/tavern.hpp"
#include "common/type.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/input.hpp"
#include "core/state.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "engine/reorder.hpp"
#include "resources/componentstore.hpp"
#include "types/character.hpp"
#include "types/component.hpp"

// Standard Constructor
Sorcery::Tavern::Tavern(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MTP::TAVERN);
	_menu->generate((*_display->layout)["tavern:menu"]);
	_menu->setPosition(_display->get_centre_x(_menu->get_width()), (*_display->layout)["tavern:menu"].y);

	// Modules
	_party_panel =
		std::make_unique<PartyPanel>(_system, _display, _graphics, _game, (*_display->layout)["global:party_panel"]);

	_stage = STV::MENU;

	_divvy = std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["tavern:dialog_divvy_gold_ok"],
		(*_display->layout)["tavern:dialog_divvy_gold_ok_text"], WDT::OK);
	_divvy->setPosition(_display->get_centre_pos(_divvy->get_size()));

	_show_divvy = false;
}

// Visit the Tavern
auto Sorcery::Tavern::start() -> std::optional<MIM> {

	using enum Enums::Castle::Tavern;
	using enum Enums::Menu::Item;

	// Do the Menus here when we has access to the game characters
	_add.reset();
	_remove.reset();

	_add = std::make_unique<Menu>(_system, _display, _graphics, _game, MTP::AVAILABLE_CHARACTERS, MMD::TAVERN);
	_add->generate((*_display->layout)["tavern_add:menu"]);
	_add->setPosition(_display->get_centre_x(_add->get_width()), (*_display->layout)["tavern_add:menu"].y);
	_remove = std::make_unique<Menu>(_system, _display, _graphics, _game, MTP::PARTY_CHARACTERS, MMD::TAVERN);
	_remove->generate((*_display->layout)["tavern_remove:menu"]);
	_remove->setPosition(_display->get_centre_x(_remove->get_width()), (*_display->layout)["tavern_remove:menu"].y);
	_inspect = std::make_unique<Inspect>(_system, _display, _graphics, _game, MMD::TAVERN);

	// Note Inspect is handled in a generic Inspect Module
	switch (_stage) {
	case MENU:
		_screen_key = "tavern";
		break;
	case ADD:
		_screen_key = "tavern_add";
		break;
	case REMOVE:
		_screen_key = "tavern_remove";
		break;
	default:
		break;
	}

	_update_menus();
	_display->generate(_screen_key);

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

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				return ITEM_ABORT;

			// Handle enabling help overlay
			if (_system->input->check(CIN::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			if (_stage == MENU) {

				// If we are in normal input mode
				if (_display->get_input_mode() == WIM::NAVIGATE_MENU) {

					if (_system->input->check(CIN::CANCEL, event))
						return std::nullopt;

					if (_system->input->check(CIN::BACK, event))
						return std::nullopt;

					if (_show_divvy) {

						if (auto dialog_input{_divvy->handle_input(event)}; dialog_input) {
							if (dialog_input.value() == WDB::CLOSE || dialog_input.value() == WDB::OK) {
								_show_divvy = false;
								_display->set_input_mode(WIM::NAVIGATE_MENU);
							}
						}

					} else {

						// And handle input on the main menu
						if (_system->input->check(CIN::UP, event))
							opt = _menu->choose_previous();
						else if (_system->input->check(CIN::DOWN, event))
							opt = _menu->choose_next();
						else if (_system->input->check(CIN::MOVE, event))
							opt = _menu->set_mouse_selected(_display->get_cur());
						else if (_system->input->check(CIN::CONFIRM, event)) {

							// We have selected something from the menu
							if (opt) {
								if (const MIM opt_tav{(*opt.value()).item}; opt_tav == TA_CASTLE) {
									return TA_CASTLE;
								} else if (opt_tav == TA_ADD_TO_PARTY) {
									_add->reload();
									_stage = ADD;
									_screen_key = "tavern_add";
									_update_menus();
									_display->generate(_screen_key);
									continue;
								} else if (opt_tav == TA_REMOVE_FROM_PARTY) {
									_remove->reload();
									_stage = REMOVE;
									_screen_key = "tavern_remove";
									_update_menus();
									_display->generate(_screen_key);
									continue;
								} else if (opt_tav == TA_INSPECT) {
									if (auto result{_inspect->start(std::nullopt)};
										result && result.value() == ITEM_ABORT) {
										_game->save_game();
										_inspect->stop();
										return ITEM_ABORT;
									}
									_inspect->stop();
									_update_menus();
									_display->generate("tavern");
									_display->set_input_mode(WIM::NAVIGATE_MENU);
									continue;
								} else if (opt_tav == TA_REORDER) {
									auto reorder{std::make_unique<Reorder>(_system, _display, _graphics, _game)};
									if (auto new_party{reorder->start()}; new_party) {
										// TODO: handle aborts here
										_game->state->set_party(new_party.value());
										_game->save_game();
										_game->load_game();
										_party_panel->refresh();
									}
									reorder->stop();
									_update_menus();
									_display->generate("tavern");
									_display->set_input_mode(WIM::NAVIGATE_MENU);
									continue;
								} else if (opt_tav == TA_DIVVY_GOLD) {

									_game->divvy_party_gold();
									_game->save_game();
									_game->load_game();

									_show_divvy = true;
									_display->set_input_mode(WIM::NAVIGATE_MENU);
								}
							}
						}
					}
				}
			} else if (_stage == ADD) {

				if (_system->input->check(CIN::CANCEL, event)) {
					_stage = MENU;
					_screen_key = "tavern";
					_update_menus();
					_display->generate(_screen_key);
					continue;
				}

				if (_system->input->check(CIN::BACK, event)) {
					_stage = MENU;
					_screen_key = "tavern";
					_update_menus();
					_display->generate(_screen_key);
					continue;
				}

				// And handle input on the main menu
				if (_system->input->check(CIN::UP, event))
					opt = _add->choose_previous();
				else if (_system->input->check(CIN::DOWN, event))
					opt = _add->choose_next();
				else if (_system->input->check(CIN::MOVE, event))
					opt = _add->set_mouse_selected(_display->get_cur());
				else if (_system->input->check(CIN::CONFIRM, event)) {

					// We have selected something from the menu
					if (opt) {
						if (const MIM opt_tav{(*opt.value()).item}; opt_tav == CA_TAVERN) {

							_stage = MENU;
							_screen_key = "tavern";
							_update_menus();
							_display->generate(_screen_key);
							continue;
						} else {

							// Add a character
							const auto character_chosen{(*opt.value()).index};
							auto &character = _game->characters[character_chosen];
							character.set_location(CHL::PARTY);
							_game->state->add_character_by_id(character_chosen);
							_game->save_game();
							_party_panel->refresh();
							_stage = MENU;
							_screen_key = "tavern";
							_update_menus();
							_display->generate(_screen_key);
						}
					}
				}
			} else if (_stage == REMOVE) {

				if (_system->input->check(CIN::CANCEL, event)) {
					_stage = MENU;
					_screen_key = "tavern";
					_update_menus();
					_display->generate(_screen_key);
					continue;
				}

				if (_system->input->check(CIN::BACK, event)) {
					_stage = MENU;
					_screen_key = "tavern";
					_update_menus();
					_display->generate(_screen_key);
					continue;
				}

				// And handle input on the main menu
				if (_system->input->check(CIN::UP, event))
					opt = _remove->choose_previous();
				else if (_system->input->check(CIN::DOWN, event))
					opt = _remove->choose_next();
				else if (_system->input->check(CIN::MOVE, event))
					opt = _remove->set_mouse_selected(_display->get_cur());
				else if (_system->input->check(CIN::CONFIRM, event)) {

					// We have selected something from the menu
					if (opt) {
						if (const MIM opt_tav{(*opt.value()).item}; opt_tav == CA_TAVERN) {

							_stage = MENU;
							_screen_key = "tavern";
							_update_menus();
							_display->generate(_screen_key);
							continue;
						} else {

							// Remove a character
							const auto character_chosen{(*opt.value()).index};
							auto &character = _game->characters[character_chosen];
							character.set_location(CHL::TAVERN);
							_game->state->remove_character_by_id(character_chosen);

							_game->save_game();
							_party_panel->refresh();
							_stage = MENU;
							_screen_key = "tavern";
							_update_menus();
							_display->generate(_screen_key);
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

auto Sorcery::Tavern::stop() -> void {
}

auto Sorcery::Tavern::_update_menus() -> void {

	Component component{(*_display->layout)["tavern:menu"]};
	if (_game->state->get_party_characters().size() == 6) {
		_menu->disable_entry(component, 0);
	} else if (_game->state->get_party_characters().size() == 0) {
		_menu->enable_entry(component, 0);
		_menu->disable_entry(component, 1);
		_menu->enable_entry(component, 2);
		_menu->disable_entry(component, 3);
		_menu->disable_entry(component, 4);
	} else {
		_menu->enable_entry(component, 0);
		_menu->enable_entry(component, 1);
		_menu->enable_entry(component, 2);
		_menu->enable_entry(component, 3);
		_menu->enable_entry(component, 4);
	}
}

auto Sorcery::Tavern::_draw() -> void {

	// Play the background movie!
	_graphics->tile_bg(_window);

	// Custom Components
	_display->display(_screen_key);
	_window->draw(*_party_panel);

	// And the Menu
	if (_stage == STV::MENU) {
		_menu->generate((*_display->layout)["tavern:menu"]);
		_window->draw(*_menu);
	} else if (_stage == STV::ADD) {
		_add->generate((*_display->layout)["tavern_add:menu"]);
		_window->draw(*_add);
	} else if (_stage == STV::REMOVE) {
		_remove->generate((*_display->layout)["tavern_remove:menu"]);
		_window->draw(*_remove);
	}

	if (_show_divvy) {
		_divvy->update();
		_window->draw(*_divvy);
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}
