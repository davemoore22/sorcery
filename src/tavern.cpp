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

#include "tavern.hpp"

// Standard Constructor
Sorcery::Tavern::Tavern(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::TAVERN);

	// Modules
	_status_bar = std::make_unique<StatusBar>(_system, _display, _graphics, _game);

	_stage = TavernStage::MENU;

	_divvy = std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["tavern:dialog_divvy_gold_ok"],
		(*_display->layout)["tavern:dialog_divvy_gold_ok_text"], WindowDialogType::OK);
	_divvy->setPosition(
		(*_display->layout)["tavern:dialog_divvy_gold_ok"].x, (*_display->layout)["tavern:dialog_divvy_gold_ok"].y);

	_show_divvy = false;
}

// Standard Destructor
Sorcery::Tavern::~Tavern() {}

// Visit the Tavern
auto Sorcery::Tavern::start() -> std::optional<MenuItem> {

	// Do the Menus here when we has access to the game characters
	_add.reset();
	_remove.reset();

	_add =
		std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::AVAILABLE_CHARACTERS, MenuMode::TAVERN);
	_remove = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::PARTY_CHARACTERS, MenuMode::TAVERN);
	_inspect = std::make_unique<Inspect>(_system, _display, _graphics, _game, MenuMode::TAVERN);

	// Note Inspect is handled in a generic Inspect Module
	switch (_stage) {
	case TavernStage::MENU:
		_screen_key = "tavern";
		break;
	case TavernStage::ADD:
		_screen_key = "tavern_add";
		break;
	case TavernStage::REMOVE:
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
	_status_bar->refresh();

	// Generate the Custom Components
	const Component status_bar_c{(*_display->layout)["status_bar:status_bar"]};
	_status_bar->setPosition(_display->window->get_x(_status_bar->sprite, status_bar_c.x),
		_display->window->get_y(_status_bar->sprite, status_bar_c.y));

	// Play the background movie!
	_display->fit_bg_movie();
	_display->start_bg_movie();

	// And do the main loop
	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
	std::optional<std::vector<MenuEntry>::const_iterator> option{_menu->items.begin()};
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				return MenuItem::ABORT;

			// Handle enabling help overlay
			if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			if (_stage == TavernStage::MENU) {

				// If we are in normal input mode
				if (_display->get_input_mode() == WindowInputMode::NAVIGATE_MENU) {

					if (_system->input->check(WindowInput::CANCEL, event))
						return std::nullopt;

					if (_system->input->check(WindowInput::BACK, event))
						return std::nullopt;

					if (_show_divvy) {

						auto dialog_input{_divvy->handle_input(event)};
						if (dialog_input) {
							if (dialog_input.value() == WindowDialogButton::CLOSE) {
								_show_divvy = false;
								_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							} else if (dialog_input.value() == WindowDialogButton::OK) {
								_show_divvy = false;
								_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							}
						};

					} else {

						// And handle input on the main menu
						if (_system->input->check(WindowInput::UP, event))
							option = _menu->choose_previous();
						else if (_system->input->check(WindowInput::DOWN, event))
							option = _menu->choose_next();
						else if (_system->input->check(WindowInput::MOVE, event))
							option =
								_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
						else if (_system->input->check(WindowInput::CONFIRM, event)) {

							// We have selected something from the menu
							if (option) {
								if (const MenuItem option_chosen{(*option.value()).item};
									option_chosen == MenuItem::TA_CASTLE) {
									return MenuItem::TA_CASTLE;
								} else if (option_chosen == MenuItem::TA_ADD_TO_PARTY) {
									_add->reload();
									_stage = TavernStage::ADD;
									_screen_key = "tavern_add";
									_update_menus();
									_display->generate(_screen_key);
									continue;
								} else if (option_chosen == MenuItem::TA_REMOVE_FROM_PARTY) {
									_remove->reload();
									_stage = TavernStage::REMOVE;
									_screen_key = "tavern_remove";
									_update_menus();
									_display->generate(_screen_key);
									continue;
								} else if (option_chosen == MenuItem::TA_INSPECT) {
									auto result{_inspect->start()};
									if (result && result.value() == MenuItem::ABORT) {
										_game->save_game();
										_inspect->stop();
										return MenuItem::ABORT;
									}
									_inspect->stop();
									_update_menus();
									_display->generate("tavern");
									_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
									continue;
								} else if (option_chosen == MenuItem::TA_REORDER) {
									auto reorder{std::make_unique<Reorder>(_system, _display, _graphics, _game)};
									auto new_party{reorder->start()};
									if (new_party) {
										// TODO: handle aborts here
										_game->state->set_party(new_party.value());
										_game->save_game();
										_game->load_game();
										_status_bar->refresh();
									}
									reorder->stop();
									_update_menus();
									_display->generate("tavern");
									_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
									continue;
								} else if (option_chosen == MenuItem::TA_DIVVY_GOLD) {

									_divvy_party_gold();
									_game->save_game();
									_game->load_game();

									_show_divvy = true;
									_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
								}
							}
						}
					}
				}
			} else if (_stage == TavernStage::ADD) {

				if (_system->input->check(WindowInput::CANCEL, event)) {
					_stage = TavernStage::MENU;
					_screen_key = "tavern";
					_update_menus();
					_display->generate(_screen_key);
					continue;
				}

				if (_system->input->check(WindowInput::BACK, event)) {
					_stage = TavernStage::MENU;
					_screen_key = "tavern";
					_update_menus();
					_display->generate(_screen_key);
					continue;
				}

				// And handle input on the main menu
				if (_system->input->check(WindowInput::UP, event))
					option = _add->choose_previous();
				else if (_system->input->check(WindowInput::DOWN, event))
					option = _add->choose_next();
				else if (_system->input->check(WindowInput::MOVE, event))
					option = _add->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
				else if (_system->input->check(WindowInput::CONFIRM, event)) {

					// We have selected something from the menu
					if (option) {
						if (const MenuItem option_chosen{(*option.value()).item};
							option_chosen == MenuItem::CA_TAVERN) {

							_stage = TavernStage::MENU;
							_screen_key = "tavern";
							_update_menus();
							_display->generate(_screen_key);
							continue;
						} else {

							// Add a character
							const auto character_chosen{(*option.value()).index};
							_game->state->add_character_by_id(character_chosen);
							_game->save_game();
							_status_bar->refresh();
							_stage = TavernStage::MENU;
							_screen_key = "tavern";
							_update_menus();
							_display->generate(_screen_key);
						}
					}
				}
			} else if (_stage == TavernStage::REMOVE) {

				if (_system->input->check(WindowInput::CANCEL, event)) {
					_stage = TavernStage::MENU;
					_screen_key = "tavern";
					_update_menus();
					_display->generate(_screen_key);
					continue;
				}

				if (_system->input->check(WindowInput::BACK, event)) {
					_stage = TavernStage::MENU;
					_screen_key = "tavern";
					_update_menus();
					_display->generate(_screen_key);
					continue;
				}

				// And handle input on the main menu
				if (_system->input->check(WindowInput::UP, event))
					option = _remove->choose_previous();
				else if (_system->input->check(WindowInput::DOWN, event))
					option = _remove->choose_next();
				else if (_system->input->check(WindowInput::MOVE, event))
					option = _remove->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
				else if (_system->input->check(WindowInput::CONFIRM, event)) {

					// We have selected something from the menu
					if (option) {
						if (const MenuItem option_chosen{(*option.value()).item};
							option_chosen == MenuItem::CA_TAVERN) {

							_stage = TavernStage::MENU;
							_screen_key = "tavern";
							_update_menus();
							_display->generate(_screen_key);
							continue;
						} else {

							// Remove a character
							const auto character_chosen{(*option.value()).index};
							_game->state->remove_character_by_id(character_chosen);

							auto &character = _game->characters[character_chosen];
							character.location = CharacterLocation::INN;
							character.coordinate = std::nullopt;
							character.depth = std::nullopt;

							_game->save_game();
							_status_bar->refresh();
							_stage = TavernStage::MENU;
							_screen_key = "tavern";
							_update_menus();
							_display->generate(_screen_key);
						}
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

auto Sorcery::Tavern::stop() -> void {

	// Stop the background movie!
	_display->stop_bg_movie();
}

auto Sorcery::Tavern::_update_menus() -> void {

	Component component{(*_display->layout)["tavern:menu"]};
	if (_game->state->get_party_characters().size() == MAX_PARTY_SIZE) {
		_menu->disable_entry(component, 0);
	} else if (_game->state->get_party_characters().size() == 0) {
		_menu->enable_entry(component, 0);
		_menu->disable_entry(component, 1);
		_menu->disable_entry(component, 2);
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

auto Sorcery::Tavern::_divvy_party_gold() -> void {

	auto party{_game->state->get_party_characters()};
	auto gold{0};

	for (auto idx : party) {
		auto _cur_char{&_game->characters.at(idx)};
		gold += _cur_char->get_gold();
	}

	gold = gold / party.size();
	for (auto idx : party) {
		auto _cur_char{&_game->characters.at(idx)};
		_cur_char->set_gold(gold);
	}
}

auto Sorcery::Tavern::_draw() -> void {

	// Custom Components
	_display->display(_screen_key);
	_window->draw(*_status_bar);

	// And the Menu
	if (_stage == TavernStage::MENU) {
		_menu->generate((*_display->layout)["tavern:menu"]);
		const sf::Vector2f menu_pos((*_display->layout)["tavern:menu"].x, (*_display->layout)["tavern:menu"].y);
		_menu->setPosition(menu_pos);
		_window->draw(*_menu);
	} else if (_stage == TavernStage::ADD) {
		_add->generate((*_display->layout)["tavern_add:menu"]);
		const sf::Vector2f menu_pos((*_display->layout)["tavern_add:menu"].x, (*_display->layout)["tavern_add:menu"].y);
		_add->setPosition(menu_pos);
		_window->draw(*_add);
	} else if (_stage == TavernStage::REMOVE) {
		_remove->generate((*_display->layout)["tavern_remove:menu"]);
		const sf::Vector2f menu_pos(
			(*_display->layout)["tavern_remove:menu"].x, (*_display->layout)["tavern_add:menu"].y);
		_remove->setPosition(menu_pos);
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
