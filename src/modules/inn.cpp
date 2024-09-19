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

#include "modules/inn.hpp"
#include "common/enum.hpp"
#include "common/type.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/input.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "resources/componentstore.hpp"
#include "resources/stringstore.hpp"
#include "types/character.hpp"
#include "types/component.hpp"

// Standard Constructor
Sorcery::Inn::Inn(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MTP::INN);
	_roster = std::make_unique<Menu>(_system, _display, _graphics, _game, MTP::PARTY_CHARACTERS, MMD::INN);
	_bed = std::make_unique<Menu>(_system, _display, _graphics, _game, MTP::INN_CHOOSE_BED);

	_menu->generate((*_display->layout)["inn:menu"]);
	_menu->setPosition(_display->get_centre_x(_menu->get_width()), (*_display->layout)["inn:menu"].y);
	_roster->generate((*_display->layout)["inn_choose:menu"]);
	_roster->setPosition(_display->get_centre_x(_roster->get_width()), (*_display->layout)["inn_choose:menu"].y);
	_bed->generate((*_display->layout)["inn_bed:menu"]);
	_bed->setPosition(_display->get_centre_x(_bed->get_width()), (*_display->layout)["inn_bed:menu"].y);

	_rest = std::make_unique<Rest>(_system, _display, _graphics, _game);
	_welcome_text = sf::Text();
	_gold_text = sf::Text();

	// Modules
	_party_panel =
		std::make_unique<PartyPanel>(_system, _display, _graphics, _game, (*_display->layout)["global:party_panel"]);
	_inspect = std::make_unique<Inspect>(_system, _display, _graphics, _game, MMD::INN);

	_stage = STI::NO_STAGE;
	_update = false;

	_pool = std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["inn:dialog_pool_gold_ok"],
		(*_display->layout)["inn:dialog_pool_gold_ok_text"], WDT::OK);
	_pool->setPosition(_display->get_centre_pos(_pool->get_size()));

	_show_pool = false;
}

// Visit the Tavern
auto Sorcery::Inn::start() -> std::optional<MIM> {

	// Get the Background Display Components and load them into Display module storage (not local - and note that due to
	// the way both menus are combined in this class, we need to have the menu stage set first in this case and this
	// case only)
	_display->generate("inn");
	_display->generate("inn_welcome", _w_sprites, _w_texts, _w_frames);
	_display->generate("inn_choose", _c_sprites, _c_texts, _c_frames);
	_display->generate("inn_bed", _b_sprites, _b_texts, _b_frames);

	// Clear the window
	_window->clear();

	// Refresh the Party characters
	_party_panel->refresh();
	_roster->reload();

	// Generate the Components
	const Component party_banel_c{(*_display->layout)["global:party_panel"]};
	_party_panel->setPosition(_display->get_centre_x(_party_panel->width), (*_display->layout)["global:party_panel"].y);

	// Start at the Menu Stage
	_stage = STI::MENU;

	// And do the main loop
	_display->set_input_mode(WIM::NAVIGATE_MENU);
	std::optional<std::vector<MenuEntry>::const_iterator> opt{_menu->items.begin()};
	std::optional<std::vector<MenuEntry>::const_iterator> opt_rost{_roster->items.begin()};
	std::optional<std::vector<MenuEntry>::const_iterator> opt_bed{_bed->items.begin()};

	// By default choose the first character in the party!
	_cur_char_id = (*opt_rost.value()).index;
	_cur_char = &_game->characters[_cur_char_id];

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

				// And handle input on the main menu
				if (_stage == STI::MENU) {

					if (_system->input->check(CIN::CANCEL, event))
						return std::nullopt;
					else if (_system->input->check(CIN::BACK, event))
						return std::nullopt;
					else if (_system->input->check(CIN::UP, event))
						opt = _menu->choose_previous();
					else if (_system->input->check(CIN::DOWN, event))
						opt = _menu->choose_next();
					else if (_system->input->check(CIN::MOVE, event))
						opt = _menu->set_mouse_selected(_display->get_cur());
					else if (_system->input->check(CIN::CONFIRM, event)) {

						// We have selected something from the menu
						if (opt) {

							if (const MIM opt_inn{(*opt.value()).item}; opt_inn == MIM::IN_CASTLE) {
								return MIM::IN_CASTLE;
							} else if (opt_inn == MIM::IN_STAY_CHARACTER) {
								_stage = STI::CHOOSE;
								_display->set_input_mode(WIM::NAVIGATE_MENU);
								_party_panel->refresh();
							} else if (opt_inn == MIM::IN_INSPECT) {
								if (auto result{_inspect->start(std::nullopt)};
									result && result.value() == MIM::ITEM_ABORT) {
									_inspect->stop();
									_game->save_game();
									_display->shutdown_SFML();
									return MIM::ITEM_ABORT;
								}
								_inspect->stop();
								_display->generate("inn");
								_display->set_input_mode(WIM::NAVIGATE_MENU);
								continue;
								if (_system->input->check(CIN::UP, event))
									opt = _menu->choose_previous();
								else if (_system->input->check(CIN::DOWN, event))
									opt = _menu->choose_next();
								else if (_system->input->check(CIN::MOVE, event))
									opt = _menu->set_mouse_selected(_display->get_cur());
								else if (_system->input->check(CIN::CONFIRM, event)) {

									// We have selected something from the menu
									if (opt) {
										if (const MIM opt_inn{(*opt.value()).item}; opt_inn == MIM::IN_CASTLE) {
											return MIM::IN_CASTLE;
										} else if (opt_inn == MIM::IN_INSPECT) {
											if (auto result{_inspect->start(std::nullopt)};
												result && result.value() == MIM::ITEM_ABORT) {
												_inspect->stop();
												_game->save_game();
												_display->shutdown_SFML();
												return MIM::ITEM_ABORT;
											}
											_inspect->stop();
											_display->generate("inn");
											_display->set_input_mode(WIM::NAVIGATE_MENU);
											continue;
										}
									}
								}
							}
						}
					}
				} else if (_stage == STI::CHOOSE) {
					if (_system->input->check(CIN::CANCEL, event)) {
						_stage = STI::MENU;
						_party_panel->refresh();
					} else if (_system->input->check(CIN::BACK, event)) {
						_stage = STI::MENU;
						_party_panel->refresh();
					} else if (_system->input->check(CIN::UP, event))
						opt_rost = _roster->choose_previous();
					else if (_system->input->check(CIN::DOWN, event))
						opt_rost = _roster->choose_next();
					else if (_system->input->check(CIN::MOVE, event))
						opt_rost = _roster->set_mouse_selected(_display->get_cur());
					else if (_system->input->check(CIN::CONFIRM, event)) {

						// We have selected something from the menu
						if (opt_rost) {
							if (const MIM opt_inn{(*opt_rost.value()).item}; opt_inn == MIM::CA_INN) {
								_stage = STI::MENU;
								_party_panel->refresh();
								continue;
							} else {
								const auto character_chosen{(*opt_rost.value()).index};
								_cur_char = &_game->characters[character_chosen];
								if (_cur_char) {
									_stage = STI::BED;
									_party_panel->refresh();
									_cur_char_id = character_chosen;
									_update = true;
								}
							}
						}
					}
				} else if (_stage == STI::BED) {

					if (_show_pool) {

						auto dialog_input{_pool->handle_input(event)};
						if (dialog_input) {
							if (dialog_input.value() == WDB::CLOSE) {
								_show_pool = false;
								_display->set_input_mode(WIM::NAVIGATE_MENU);
							} else if (dialog_input.value() == WDB::OK) {
								_show_pool = false;
								_display->set_input_mode(WIM::NAVIGATE_MENU);
							}
						}
					} else {

						_update = true;
						if (_system->input->check(CIN::CANCEL, event)) {
							_stage = STI::CHOOSE;
							_party_panel->refresh();
						} else if (_system->input->check(CIN::BACK, event)) {
							_stage = STI::CHOOSE;
							_party_panel->refresh();
						} else if (_system->input->check(CIN::UP, event))
							opt_bed = _bed->choose_previous();
						else if (_system->input->check(CIN::DOWN, event))
							opt_bed = _bed->choose_next();
						else if (_system->input->check(CIN::MOVE, event))
							opt_bed = _bed->set_mouse_selected(_display->get_cur());
						else if (_system->input->check(CIN::CONFIRM, event)) {

							// We have selected something from the menu
							if (opt_bed) {
								if (const MIM opt_inn{(*opt_bed.value()).item}; opt_inn == MIM::IN_BACK) {
									_stage = STI::CHOOSE;
									_party_panel->refresh();
									continue;
								} else if (opt_inn == MIM::IN_POOL_GOLD) {

									_game->pool_party_gold(_cur_char_id);
									_game->save_game();

									_show_pool = true;
									_display->set_input_mode(WIM::NAVIGATE_MENU);
								} else if (opt_inn == MIM::IN_STABLES) {
									if (auto opt_stab{_rest->start(_cur_char.value(), REM::SINGLE, RET::STABLES)};
										opt_stab && opt_stab.value() == MIM::ITEM_ABORT) {
										_game->save_game();
										_display->shutdown_SFML();
										return MIM::ITEM_ABORT;
									}
									_rest->stop();
									_roster->reload();
									_display->generate("inn");
									_party_panel->refresh();
									continue;
								} else if (opt_inn == MIM::IN_COT) {
									if (auto opt_stab{_rest->start(_cur_char.value(), REM::SINGLE, RET::COT)};
										opt_stab && opt_stab.value() == MIM::ITEM_ABORT) {
										_game->save_game();
										_display->shutdown_SFML();
										return MIM::ITEM_ABORT;
									}
									_rest->stop();
									_roster->reload();
									_display->generate("inn");
									_party_panel->refresh();
									continue;
								} else if (opt_inn == MIM::IN_ECONOMY) {
									if (auto opt_stab{_rest->start(_cur_char.value(), REM::SINGLE, RET::ECONOMY)};
										opt_stab && opt_stab.value() == MIM::ITEM_ABORT) {
										_game->save_game();
										_display->shutdown_SFML();
										return MIM::ITEM_ABORT;
									}
									_rest->stop();
									_roster->reload();
									_display->generate("inn");
									_party_panel->refresh();
									continue;
								} else if (opt_inn == MIM::IN_MERCHANT) {
									if (auto opt_stab{_rest->start(_cur_char.value(), REM::SINGLE, RET::MERCHANT)};
										opt_stab && opt_stab.value() == MIM::ITEM_ABORT) {
										_game->save_game();
										_display->shutdown_SFML();
										return MIM::ITEM_ABORT;
									}
									_rest->stop();
									_roster->reload();
									_display->generate("inn");
									_party_panel->refresh();
									continue;
								} else if (opt_inn == MIM::IN_ROYAL) {
									if (auto opt_stab{_rest->start(_cur_char.value(), REM::SINGLE, RET::ROYAL)};
										opt_stab && opt_stab.value() == MIM::ITEM_ABORT) {
										_game->save_game();
										_display->shutdown_SFML();
										return MIM::ITEM_ABORT;
									}
									_rest->stop();
									_roster->reload();
									_display->generate("inn");
									_party_panel->refresh();
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

auto Sorcery::Inn::stop() -> void {
}

auto Sorcery::Inn::_update_and_draw_bed_screen() -> void {

	if (_update) {
		auto name{_cur_char.value()->get_name()};
		const auto welcome{
			fmt::format("{} {}. {}", (*_display->string)["INN_WELCOME_ONE"], name, (*_display->string)["INN_HAVE"])};
		const auto gp{_cur_char.value()->get_gold()};
		const auto gold{fmt::format("{} {} {}.", (*_display->string)["INN_YOU"], gp, (*_display->string)["INN_GP"])};

		_display->window->draw_text(_gold_text, (*_display->layout)["inn_bed:gold_text"], gold);
		_display->window->draw_text(_welcome_text, (*_display->layout)["inn_bed:welcome_text"], welcome);
		_update = false;

		Component component{(*_display->layout)["inn_bed:menu"]};
		auto cur_gold{_cur_char.value()->get_gold()};
		_menu->enable_entry(component, 0);
		if (cur_gold >= 10)
			_bed->enable_entry(component, 1);
		else
			_bed->disable_entry(component, 1);
		if (cur_gold >= 50)
			_bed->enable_entry(component, 2);
		else
			_bed->disable_entry(component, 2);
		if (cur_gold >= 200)
			_bed->enable_entry(component, 3);
		else
			_bed->disable_entry(component, 3);
		if (cur_gold >= 500)
			_bed->enable_entry(component, 4);
		else
			_bed->disable_entry(component, 4);

	} else {

		_window->draw(_gold_text);
		_window->draw(_welcome_text);
	}
}

auto Sorcery::Inn::_draw() -> void {

	// Background Wallpaper
	_graphics->tile_bg(_window);

	// Custom Components
	_display->display("inn");
	_window->draw(*_party_panel);

	if (_stage == STI::MENU) {

		// And the Menu
		_menu->generate((*_display->layout)["inn:menu"]);
		_display->display("inn_welcome", _w_sprites, _w_texts, _w_frames);
		_window->draw(*_menu);

	} else if (_stage == STI::CHOOSE) {

		// And the Menu
		_roster->generate((*_display->layout)["inn_choose:menu"]);
		_display->display("inn_choose", _c_sprites, _c_texts, _c_frames);
		_window->draw(*_roster);
	} else if (_stage == STI::BED) {

		// And the Menu
		_bed->generate((*_display->layout)["inn_bed:menu"]);
		_display->display("inn_bed", _b_sprites, _b_texts, _b_frames);
		_window->draw(*_bed);

		_update_and_draw_bed_screen();

		if (_show_pool) {
			_pool->update();
			_window->draw(*_pool);
		}
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}
