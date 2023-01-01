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

#include "temple.hpp"

// Standard Constructor
Sorcery::Temple::Temple(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::TEMPLE);
	_help = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::INVALID_CHARACTERS, MenuMode::TEMPLE);
	_pay = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::PARTY_CHARACTERS, MenuMode::TEMPLE);

	// Modules
	_status_bar = std::make_unique<StatusBar>(_system, _display, _graphics, _game);
	_inspect = std::make_unique<Inspect>(_system, _display, _graphics, _game, MenuMode::TEMPLE);

	_stage = TempleStage::NONE;

	_cost = sf::Text();
}

// Standard Destructor
Sorcery::Temple::~Temple() {
}

// Visit the Tavern
auto Sorcery::Temple::start() -> std::optional<MenuItem> {

	// Get the Background Display Components and load them into Display module
	// storage (not local - and note that due to the way both menus are combined
	// in this class, we need to have the menu stage set first in this case and
	// this case only)
	_display->generate("temple");
	_display->generate("temple_help", _h_sprites, _h_texts, _h_frames);
	_display->generate("temple_pay", _p_sprites, _p_texts, _p_frames);

	// Clear the window
	_window->clear();

	// Refresh the Party characters and the Invalid Characters
	_status_bar->refresh();
	_help->reload();

	// Generate the Components
	const Component status_bar_c{(*_display->layout)["status_bar:status_bar"]};
	_status_bar->setPosition(_display->window->get_x(_status_bar->sprite, status_bar_c.x),
		_display->window->get_y(_status_bar->sprite, status_bar_c.y));

	// Play the background movie!
	_display->fit_bg_movie();
	_display->start_bg_movie();

	// Start at the entry menu
	_stage = TempleStage::MENU;

	// And do the main loop
	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
	std::optional<std::vector<MenuEntry>::const_iterator> option{_menu->items.begin()};
	std::optional<std::vector<MenuEntry>::const_iterator> option_help{_help->items.begin()};
	std::optional<std::vector<MenuEntry>::const_iterator> option_pay{_pay->items.begin()};
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// If we are in normal input mode
			if (_display->get_input_mode() == WindowInputMode::NAVIGATE_MENU) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return MenuItem::ABORT;

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
				if (_stage == TempleStage::MENU) {
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
								option_chosen == MenuItem::TE_CASTLE) {
								return MenuItem::TE_CASTLE;
							} else if (option_chosen == MenuItem::TE_INSPECT) {
								auto result{_inspect->start()};
								if (result && result.value() == MenuItem::ABORT) {
									_inspect->stop();
									return MenuItem::ABORT;
								}
								_inspect->stop();
								_display->generate("temple");
								_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
								continue;
							} else if (const MenuItem option_chosen{(*option.value()).item};
									   option_chosen == MenuItem::TE_HELP) {
								_stage = TempleStage::HELP;
								_status_bar->refresh();
								_help->reload();
								continue;
							}
						}
					}
				} else if (_stage == TempleStage::HELP) {
					if (_system->input->check(WindowInput::CANCEL, event)) {
						_stage = TempleStage::MENU;
						_status_bar->refresh();
						_help->reload();
					} else if (_system->input->check(WindowInput::BACK, event)) {
						_stage = TempleStage::MENU;
						_status_bar->refresh();
						_help->reload();
					} else if (_system->input->check(WindowInput::UP, event))
						option_help = _help->choose_previous();
					else if (_system->input->check(WindowInput::DOWN, event))
						option_help = _help->choose_next();
					else if (_system->input->check(WindowInput::MOVE, event))
						option_help =
							_help->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					else if (_system->input->check(WindowInput::CONFIRM, event)) {

						// We have selected something from the menu
						if (option_help) {
							if (const MenuItem option_chosen{(*option_help.value()).item};
								option_chosen == MenuItem::CA_TEMPLE) {
								_stage = TempleStage::MENU;
								_status_bar->refresh();
								_help->reload();
								continue;
							} else {
								const auto character_chosen{(*option_help.value()).index};
								_cur_char = &_game->characters.at(character_chosen);
								const auto cost{_cur_char.value()->get_cure_cost()};
								if (_cur_char) {
									_stage = TempleStage::PAY;
									_status_bar->refresh();
									_refresh_pay_menu(cost);
									_update_cost(cost);
									_cur_char_id = character_chosen;
								}
							}
						}
					}
				} else if (_stage == TempleStage::PAY) {

					if (_system->input->check(WindowInput::CANCEL, event)) {
						_stage = TempleStage::HELP;
						_status_bar->refresh();
						_help->reload();
					} else if (_system->input->check(WindowInput::BACK, event)) {
						_stage = TempleStage::HELP;
						_status_bar->refresh();
						_help->reload();
					} else if (_system->input->check(WindowInput::UP, event))
						option_pay = _pay->choose_previous();
					else if (_system->input->check(WindowInput::DOWN, event))
						option_pay = _pay->choose_next();
					else if (_system->input->check(WindowInput::MOVE, event))
						option_pay =
							_pay->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					else if (_system->input->check(WindowInput::CONFIRM, event)) {

						// We have selected something from the menu
						if (option_help) {
							if (const MenuItem option_chosen{(*option_pay.value()).item};
								option_chosen == MenuItem::CA_TEMPLE) {
								_stage = TempleStage::HELP;
								_status_bar->refresh();
								_help->reload();
								continue;
							} else {
								const auto character_chosen{(*option_pay.value()).index};
								_cur_char = &_game->characters.at(character_chosen);
								/* const auto cost{_cur_char.value()->get_cure_cost()};
								if (_cur_char) {
									_stage = TempleStage::PAY;
									_status_bar->refresh();
									_refresh_pay_menu();
									_cur_char_id = character_chosen;
								} */
							}
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

auto Sorcery::Temple::stop() -> void {

	// Stop the background movie!
	_display->stop_bg_movie();
}

auto Sorcery::Temple::_update_cost(const unsigned int cost) -> void {

	_cost_text = fmt::format(
		"{} {} {}", (*_display->string)["TEMPLE_COST_PREFIX"], cost, (*_display->string)["TEMPLE_COST_SUFFIX"]);
}

auto Sorcery::Temple::_refresh_pay_menu(const unsigned int cost) -> void {

	// disable any character menu item who doesn't have enough gold
}

auto Sorcery::Temple::_draw() -> void {

	// Custom Components
	_display->display("temple");
	_window->draw(*_status_bar);

	if (_stage == TempleStage::MENU) {

		// And the Menu
		_menu->generate((*_display->layout)["temple:menu"]);
		const sf::Vector2f menu_pos((*_display->layout)["temple:menu"].x, (*_display->layout)["temple:menu"].y);
		_menu->setPosition(menu_pos);
		_window->draw(*_menu);
	} else if (_stage == TempleStage::HELP) {

		// Choose Invalid Character
		_help->generate((*_display->layout)["temple_help:menu"]);
		const sf::Vector2f menu_pos(
			(*_display->layout)["temple_help:menu"].x, (*_display->layout)["temple_help:menu"].y);
		_help->setPosition(menu_pos);
		_display->display("inn_choose", _h_sprites, _h_texts, _h_frames);
		_window->draw(*_help);
	} else if (_stage == TempleStage::PAY) {

		_pay->generate((*_display->layout)["temple_pay:menu"]);
		const sf::Vector2f menu_pos(
			(*_display->layout)["temple_help:menu"].x, (*_display->layout)["temple_pay:menu"].y);
		_pay->setPosition(menu_pos);
		_display->display("inn_pay", _p_sprites, _p_texts, _p_frames);
		_display->window->draw_text(_cost, (*_display->layout)["temple_pay:cost_text"], _cost_text);
		_window->draw(*_pay);
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}
